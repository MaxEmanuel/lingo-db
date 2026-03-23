#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

#include "execution/Execution.h"
#include "execution/Frontend.h"
#include "frontend/SQL/Parser.h"
#include "json.h"
#include "mlir-support/eval.h"
#include "runtime/Catalog.h"
#include "runtime/TableBuilder.h"

#include "mlir/Conversion/DBToStd/DBToStd.h"
#include "mlir/Conversion/DSAToStd/DSAToStd.h"
#include "mlir/Conversion/RelAlgToSubOp/RelAlgToSubOpPass.h"
#include "mlir/Conversion/SubOpToControlFlow/SubOpToControlFlowPass.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/RelAlg/Passes.h"
#include "mlir/Dialect/SubOperator/SubOperatorOps.h"
#include "mlir/Dialect/SubOperator/Transforms/Passes.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Transforms/Passes.h"

#define CPPHTTPLIB_THREAD_POOL_COUNT 4
#include "httplib.h"

#include <arrow/api.h>
#include <arrow/table.h>

// Mutex to serialize query execution (the MLIR pipeline is not thread-safe)
static std::mutex executionMutex;

static std::string moduleToString(mlir::ModuleOp moduleOp) {
   std::string result;
   llvm::raw_string_ostream rso(result);
   mlir::OpPrintingFlags flags;
   flags.assumeVerified();
   moduleOp->print(rso, flags);
   return result;
}

static std::string translateToMLIR(const std::string& sql, runtime::Catalog& catalog) {
   mlir::MLIRContext context;
   execution::initializeContext(context);

   mlir::OpBuilder builder(&context);
   mlir::ModuleOp moduleOp = builder.create<mlir::ModuleOp>(builder.getUnknownLoc());
   frontend::sql::Parser translator(sql, catalog, moduleOp);

   builder.setInsertionPointToStart(moduleOp.getBody());
   auto* queryBlock = new mlir::Block;
   {
      mlir::OpBuilder::InsertionGuard guard(builder);
      builder.setInsertionPointToStart(queryBlock);
      auto val = translator.translate(builder);
      if (val.has_value()) {
         builder.create<mlir::subop::SetResultOp>(builder.getUnknownLoc(), 0, val.value());
      }
      builder.create<mlir::func::ReturnOp>(builder.getUnknownLoc());
   }
   mlir::func::FuncOp funcOp = builder.create<mlir::func::FuncOp>(
      builder.getUnknownLoc(), "main", builder.getFunctionType({}, {}));
   funcOp.getBody().push_back(queryBlock);

   return moduleToString(moduleOp);
}

static nlohmann::json generatePipelineStages(const std::string& sql, runtime::Session& session) {
   nlohmann::json stages = nlohmann::json::array();
   auto catalog = session.getCatalog();

   mlir::MLIRContext context;
   execution::initializeContext(context);

   mlir::OpBuilder builder(&context);
   mlir::ModuleOp moduleOp = builder.create<mlir::ModuleOp>(builder.getUnknownLoc());
   frontend::sql::Parser translator(sql, *catalog, moduleOp);

   builder.setInsertionPointToStart(moduleOp.getBody());
   auto* queryBlock = new mlir::Block;
   {
      mlir::OpBuilder::InsertionGuard guard(builder);
      builder.setInsertionPointToStart(queryBlock);
      auto val = translator.translate(builder);
      if (val.has_value()) {
         builder.create<mlir::subop::SetResultOp>(builder.getUnknownLoc(), 0, val.value());
      }
      builder.create<mlir::func::ReturnOp>(builder.getUnknownLoc());
   }
   mlir::func::FuncOp funcOp = builder.create<mlir::func::FuncOp>(
      builder.getUnknownLoc(), "main", builder.getFunctionType({}, {}));
   funcOp.getBody().push_back(queryBlock);

   // Stage 1: Canonical RelAlg
   stages.push_back({{"name", "Canonical (RelAlg)"}, {"mlir", moduleToString(moduleOp)}});

   // Stage 2: Optimized RelAlg
   {
      mlir::PassManager pm(&context);
      mlir::relalg::createQueryOptPipeline(pm, catalog.get());
      if (pm.run(moduleOp).succeeded()) {
         stages.push_back({{"name", "Optimized (RelAlg)"}, {"mlir", moduleToString(moduleOp)}});
      } else {
         stages.push_back({{"name", "Optimized (RelAlg)"}, {"mlir", "// Pass failed"}});
      }
   }

   // Stage 3: SubOperator
   {
      mlir::PassManager pm(&context);
      mlir::relalg::createLowerRelAlgToSubOpPipeline(pm);
      if (pm.run(moduleOp).succeeded()) {
         // Load tables needed by the module
         moduleOp.walk([&](mlir::Operation* op) {
            if (auto getExternalOp = mlir::dyn_cast_or_null<mlir::subop::GetExternalOp>(op)) {
               auto json = nlohmann::json::parse(getExternalOp.getDescr().str());
               auto tableName = json.value("table", "");
               bool addIndex = json.contains("index");
               if (!tableName.size()) {
                  if (!addIndex) return;
                  tableName = json["relation"];
               }
               if (auto relation = catalog->findRelation(tableName)) {
                  relation->loadData();
                  if (addIndex) relation->getIndex(json["index"])->ensureLoaded();
               }
            }
         });
         stages.push_back({{"name", "SubOperator"}, {"mlir", moduleToString(moduleOp)}});
      } else {
         stages.push_back({{"name", "SubOperator"}, {"mlir", "// Pass failed"}});
      }
   }

   // Stage 4: HL Imperative (SubOp lowering)
   {
      moduleOp->setAttr("subop.sequential", mlir::UnitAttr::get(moduleOp->getContext()));
      mlir::PassManager pm(&context);
      pm.addPass(mlir::subop::createGlobalOptPass());
      pm.addPass(mlir::subop::createFoldColumnsPass());
      pm.addPass(mlir::subop::createReuseLocalPass());
      pm.addPass(mlir::subop::createSpecializeSubOpPass(true));
      pm.addPass(mlir::subop::createNormalizeSubOpPass());
      pm.addPass(mlir::subop::createPullGatherUpPass());
      pm.addPass(mlir::subop::createEnforceOrderPass());
      pm.addPass(mlir::subop::createLowerSubOpPass());
      pm.addPass(mlir::createCanonicalizerPass());
      pm.addPass(mlir::createCSEPass());
      if (pm.run(moduleOp).succeeded()) {
         stages.push_back({{"name", "HL Imperative"}, {"mlir", moduleToString(moduleOp)}});
      } else {
         stages.push_back({{"name", "HL Imperative"}, {"mlir", "// Pass failed"}});
      }
   }

   // Stage 5: LL Imperative (DB + DSA lowering)
   {
      mlir::PassManager pm(&context);
      mlir::db::createLowerDBPipeline(pm);
      pm.addPass(mlir::dsa::createLowerToStdPass());
      pm.addPass(mlir::createCanonicalizerPass());
      pm.addPass(mlir::createLoopInvariantCodeMotionPass());
      pm.addPass(mlir::createCSEPass());
      if (pm.run(moduleOp).succeeded()) {
         stages.push_back({{"name", "LL Imperative"}, {"mlir", moduleToString(moduleOp)}});
      } else {
         stages.push_back({{"name", "LL Imperative"}, {"mlir", "// Pass failed"}});
      }
   }

   return stages;
}

static std::string arrowValueToString(const std::shared_ptr<arrow::Array>& array, int64_t idx) {
   if (array->IsNull(idx)) return "null";

   switch (array->type_id()) {
      case arrow::Type::INT8: return std::to_string(std::static_pointer_cast<arrow::Int8Array>(array)->Value(idx));
      case arrow::Type::INT16: return std::to_string(std::static_pointer_cast<arrow::Int16Array>(array)->Value(idx));
      case arrow::Type::INT32: return std::to_string(std::static_pointer_cast<arrow::Int32Array>(array)->Value(idx));
      case arrow::Type::INT64: return std::to_string(std::static_pointer_cast<arrow::Int64Array>(array)->Value(idx));
      case arrow::Type::FLOAT: return std::to_string(std::static_pointer_cast<arrow::FloatArray>(array)->Value(idx));
      case arrow::Type::DOUBLE: return std::to_string(std::static_pointer_cast<arrow::DoubleArray>(array)->Value(idx));
      case arrow::Type::STRING: return std::static_pointer_cast<arrow::StringArray>(array)->GetString(idx);
      case arrow::Type::BOOL: return std::static_pointer_cast<arrow::BooleanArray>(array)->Value(idx) ? "true" : "false";
      case arrow::Type::HALF_FLOAT: {
         uint16_t raw = std::static_pointer_cast<arrow::HalfFloatArray>(array)->Value(idx);
         float val;
         uint32_t sign = (raw >> 15) & 1;
         uint32_t exp = (raw >> 10) & 0x1F;
         uint32_t mant = raw & 0x3FF;
         if (exp == 0) {
            val = (sign ? -1.0f : 1.0f) * (mant / 1024.0f) * std::pow(2.0f, -14.0f);
         } else if (exp == 31) {
            val = mant ? std::numeric_limits<float>::quiet_NaN() : (sign ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity());
         } else {
            val = (sign ? -1.0f : 1.0f) * (1.0f + mant / 1024.0f) * std::pow(2.0f, (int)exp - 15);
         }
         return std::to_string(val);
      }
      default: {
         auto result = array->GetScalar(idx);
         if (result.ok()) {
            return result.ValueOrDie()->ToString();
         }
         return "?";
      }
   }
}

static nlohmann::json arrowTableToJson(const std::shared_ptr<arrow::Table>& table) {
   nlohmann::json result;
   nlohmann::json columns = nlohmann::json::array();
   nlohmann::json rows = nlohmann::json::array();

   for (int i = 0; i < table->num_columns(); i++) {
      columns.push_back(table->schema()->field(i)->name());
   }

   auto combined = table->CombineChunks();
   if (!combined.ok()) {
      result["columns"] = columns;
      result["rows"] = rows;
      return result;
   }
   auto combinedTable = combined.ValueOrDie();

   for (int64_t row = 0; row < combinedTable->num_rows(); row++) {
      nlohmann::json rowObj = nlohmann::json::array();
      for (int col = 0; col < combinedTable->num_columns(); col++) {
         auto chunk = combinedTable->column(col)->chunk(0);
         rowObj.push_back(arrowValueToString(chunk, row));
      }
      rows.push_back(rowObj);
   }

   result["columns"] = columns;
   result["rows"] = rows;
   return result;
}

// ---------------------------------------------------------------------------
// Embedded HTML/CSS/JS frontend
// ---------------------------------------------------------------------------
static const char* HTML_PAGE = R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>∂SQL — SQL & MLIR Explorer</title>
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.css">
<script defer src="https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.js"></script>
<style>
*{margin:0;padding:0;box-sizing:border-box;}
:root{
  --bg:#1e1e2e;--surface:#181825;--overlay:#313244;--text:#cdd6f4;
  --subtext:#a6adc8;--muted:#6c7086;--blue:#89b4fa;--teal:#74c7ec;
  --green:#a6e3a1;--red:#f38ba8;--yellow:#f9e2af;--mauve:#cba6f7;
  --peach:#fab387;--pink:#f5c2e7;--crust:#11111b;--mantle:#181825;
  --font-mono:'JetBrains Mono','Fira Code','Cascadia Code',monospace;
}
[data-theme="light"]{
  --bg:#eff1f5;--surface:#e6e9ef;--overlay:#ccd0da;--text:#4c4f69;
  --subtext:#5c5f77;--muted:#8c8fa1;--blue:#1e66f5;--teal:#179299;
  --green:#40a02b;--red:#d20f39;--yellow:#df8e1d;--mauve:#8839ef;
  --peach:#fe640b;--pink:#ea76cb;--crust:#dce0e8;--mantle:#e6e9ef;
}
html,body{height:100%;overflow:hidden;}
body{font-family:'Segoe UI',system-ui,-apple-system,sans-serif;background:var(--bg);color:var(--text);display:flex;flex-direction:column;height:100vh;}

/* Header */
header{background:var(--surface);padding:8px 16px;display:flex;align-items:center;gap:12px;border-bottom:1px solid var(--overlay);flex-shrink:0;}
header h1{font-size:16px;color:var(--blue);font-weight:700;letter-spacing:-0.3px;}
header .sep{color:var(--overlay);font-size:14px;}
header .subtitle{font-size:12px;color:var(--muted);}
.toolbar{display:flex;gap:6px;margin-left:auto;align-items:center;}
.toolbar button{border:none;padding:5px 14px;border-radius:5px;cursor:pointer;font-size:12px;font-weight:600;transition:background .15s;}
.btn-primary{background:var(--blue);color:var(--crust);}
.btn-primary:hover{background:var(--teal);}
.btn-secondary{background:var(--overlay);color:var(--text);}
.btn-secondary:hover{background:#45475a;}
[data-theme="light"] .btn-secondary:hover{background:#bcc0cc;}

/* Grid */
.grid{display:grid;grid-template-columns:1fr 1fr;grid-template-rows:1fr 1fr;flex:1;overflow:hidden;gap:1px;background:var(--overlay);}
.quadrant{background:var(--bg);display:flex;flex-direction:column;overflow:hidden;min-width:0;min-height:0;}

/* Panel headers */
.panel-header{padding:6px 12px;background:var(--surface);font-size:11px;font-weight:600;color:var(--subtext);text-transform:uppercase;letter-spacing:.6px;border-bottom:1px solid var(--overlay);display:flex;align-items:center;gap:8px;flex-shrink:0;}

/* SQL Tabs */
.tabs-bar{display:flex;align-items:center;background:var(--surface);border-bottom:1px solid var(--overlay);flex-shrink:0;overflow-x:auto;min-height:32px;}
.sql-tab{padding:6px 14px;font-size:12px;cursor:pointer;color:var(--muted);border-bottom:2px solid transparent;white-space:nowrap;position:relative;user-select:none;}
.sql-tab:hover{color:var(--subtext);}
.sql-tab.active{color:var(--blue);border-bottom-color:var(--blue);}
.sql-tab .close-tab{margin-left:6px;font-size:10px;opacity:.4;cursor:pointer;}
.sql-tab .close-tab:hover{opacity:1;color:var(--red);}
.add-tab{background:none;border:none;color:var(--muted);font-size:16px;cursor:pointer;padding:4px 10px;line-height:1;}
.add-tab:hover{color:var(--blue);}
.sql-editor{flex:1;background:var(--bg);color:var(--text);border:none;padding:12px;font-family:var(--font-mono);font-size:13px;resize:none;outline:none;line-height:1.6;tab-size:2;}
.sql-editor::placeholder{color:#585b70;}

/* SQL editor with syntax highlighting overlay */
.editor-wrap{position:relative;flex:1;overflow:hidden;}
.editor-wrap textarea,.editor-wrap pre{margin:0;padding:12px;font-family:var(--font-mono);font-size:13px;line-height:1.6;tab-size:2;border:none;white-space:pre-wrap;word-wrap:break-word;overflow-wrap:break-word;}
.editor-wrap textarea{position:absolute;inset:0;width:100%;height:100%;resize:none;background:transparent;color:transparent;caret-color:var(--text);outline:none;z-index:2;-webkit-text-fill-color:transparent;}
.editor-wrap pre{position:absolute;inset:0;width:100%;height:100%;overflow:auto;pointer-events:none;z-index:1;color:var(--text);background:var(--bg);}
.sql-kw{color:var(--mauve);font-weight:600;}
.sql-fn{color:var(--blue);}
.sql-str{color:var(--green);}
.sql-num{color:var(--peach);}
.sql-comment{color:var(--muted);font-style:italic;}
.sql-type{color:var(--yellow);}
.sql-op{color:var(--red);}

/* Code toolbar */
.code-toolbar{display:flex;align-items:center;gap:8px;padding:5px 10px;background:var(--surface);border-bottom:1px solid var(--overlay);flex-shrink:0;flex-wrap:wrap;}
.code-toolbar select{background:var(--overlay);color:var(--text);border:1px solid #45475a;padding:3px 8px;border-radius:4px;font-size:12px;outline:none;}
.code-toolbar label{font-size:12px;color:var(--subtext);cursor:pointer;display:flex;align-items:center;gap:4px;}
.code-toolbar input[type="checkbox"]{accent-color:var(--blue);}
.code-view{flex:1;overflow:auto;position:relative;}
.code-output{padding:12px;font-family:var(--font-mono);font-size:12px;line-height:1.55;white-space:pre;color:var(--text);margin:0;overflow:auto;height:100%;}

/* Compare side-by-side */
.compare-container{display:flex;height:100%;flex:1;overflow:hidden;}
.compare-pane{flex:1;overflow:auto;display:flex;flex-direction:column;}
.compare-pane+.compare-pane{border-left:1px solid var(--overlay);}
.compare-label{padding:4px 10px;font-size:11px;font-weight:600;color:var(--muted);background:var(--crust);text-transform:uppercase;letter-spacing:.5px;flex-shrink:0;}

/* Results table */
.results-split{display:flex;flex:1;overflow:hidden;}
.results-table-pane{flex:1;overflow:auto;border-right:1px solid var(--overlay);min-width:0;}
.results-plot-pane{flex:1;overflow:hidden;min-width:0;background:var(--bg);display:flex;align-items:center;justify-content:center;padding:8px;}
.results-container{flex:1;overflow:auto;}
.results-table{width:100%;border-collapse:collapse;font-size:12px;font-family:var(--font-mono);}
.results-table th{background:var(--surface);padding:6px 10px;text-align:left;border-bottom:2px solid var(--overlay);color:var(--blue);position:sticky;top:0;z-index:1;}
.results-table td{padding:5px 10px;border-bottom:1px solid var(--overlay);}
.results-table tr:hover td{background:var(--overlay);}

/* Plot */
.plot-axis{stroke:var(--muted);stroke-width:1;}
.plot-tick{font-size:9px;fill:var(--subtext);font-family:var(--font-mono);}
.plot-label{font-size:10px;fill:var(--text);font-weight:600;font-family:var(--font-mono);}
.plot-grid{stroke:var(--overlay);stroke-width:0.5;stroke-dasharray:3,3;}
.plot-line{fill:none;stroke:var(--blue);stroke-width:2;stroke-linecap:round;stroke-linejoin:round;}
.plot-dot{fill:var(--blue);stroke:var(--bg);stroke-width:1.5;}
.plot-empty{font-size:11px;fill:var(--muted);text-anchor:middle;dominant-baseline:central;}

/* Timing */
.timing-container{flex:1;overflow:auto;padding:10px;}
.timing-bar-group{margin-bottom:6px;}
.timing-label{font-size:11px;color:var(--subtext);margin-bottom:2px;display:flex;justify-content:space-between;}
.timing-bar-track{height:16px;background:var(--overlay);border-radius:3px;overflow:hidden;position:relative;}
.timing-bar-fill{height:100%;border-radius:3px;transition:width .3s ease;}
.timing-summary{margin-top:12px;border-top:1px solid var(--overlay);padding-top:8px;}
.timing-summary table{width:100%;font-size:11px;border-collapse:collapse;font-family:var(--font-mono);}
.timing-summary td,.timing-summary th{padding:3px 6px;}
.timing-summary td:first-child,.timing-summary th:first-child{color:var(--subtext);}
.timing-summary td:nth-child(n+2),.timing-summary th:nth-child(n+2){text-align:right;color:var(--text);}
.timing-compare-pair{display:flex;gap:2px;flex-direction:column;}
.timing-compare-pair .timing-bar-track{height:10px;}
.timing-mode-legend{display:flex;gap:14px;margin-bottom:8px;font-size:11px;font-weight:600;}
.timing-mode-legend span{display:flex;align-items:center;gap:5px;}
.timing-mode-legend .swatch{width:10px;height:10px;border-radius:2px;display:inline-block;}
.timing-speedup{font-weight:700;font-size:11px;}
.timing-speedup.faster{color:var(--green);}
.timing-speedup.slower{color:var(--red);}
.timing-speedup.same{color:var(--muted);}

/* Status */
.status-bar{background:var(--surface);padding:4px 14px;font-size:11px;color:var(--muted);border-top:1px solid var(--overlay);display:flex;justify-content:space-between;flex-shrink:0;}

/* Utility */
.error{color:var(--red);padding:12px;font-family:var(--font-mono);font-size:12px;white-space:pre-wrap;}
.info{color:var(--green);padding:12px;font-family:var(--font-mono);font-size:12px;}
.loading{color:var(--yellow);padding:12px;font-size:12px;}

/* Syntax highlighting */
.hl-keyword{color:var(--mauve);font-weight:600;}
.hl-type{color:var(--yellow);}
.hl-op{color:var(--blue);}
.hl-attr{color:var(--teal);}
.hl-string{color:var(--green);}
.hl-number{color:var(--peach);}
.hl-comment{color:var(--muted);font-style:italic;}
.hl-deriv-line{background:rgba(203,166,247,0.12);display:inline-block;width:100%;border-left:3px solid var(--mauve);padding-left:6px;margin-left:-9px;}
[data-theme="light"] .hl-deriv-line{background:rgba(136,57,239,0.08);}
.hl-expr-line{background:rgba(137,180,250,0.10);display:inline-block;width:100%;border-left:3px solid var(--blue);padding-left:6px;margin-left:-9px;}
[data-theme="light"] .hl-expr-line{background:rgba(30,102,245,0.06);}
.hl-deriv-attr{color:var(--mauve);font-weight:700;}
.hl-op-mul{color:var(--peach);font-weight:600;}
.hl-op-add{color:var(--green);font-weight:600;}
.hl-op-sub{color:var(--red);font-weight:600;}
.hl-op-div{color:var(--mauve);font-weight:600;}
.hl-op-cmp{color:var(--flamingo);font-weight:600;}
.hl-runtime-fn{color:var(--sky);font-weight:700;}
.hl-math-badge{display:inline-block;font-size:10px;padding:0 3px;border-radius:3px;margin-left:2px;vertical-align:middle;font-weight:700;line-height:14px;}
.hl-badge-mul{background:rgba(250,179,135,0.2);color:var(--peach);}
.hl-badge-add{background:rgba(166,227,161,0.2);color:var(--green);}
.hl-badge-sub{background:rgba(243,139,168,0.2);color:var(--red);}
.hl-badge-pow{background:rgba(137,180,250,0.2);color:var(--blue);}
.hl-badge-sig{background:rgba(148,226,213,0.2);color:var(--teal);}
.hl-badge-log{background:rgba(249,226,175,0.2);color:var(--yellow);}
.hl-badge-mmul{background:rgba(245,194,231,0.2);color:var(--pink);}
.hl-badge-transp{background:rgba(180,190,254,0.2);color:var(--lavender);}

/* Cursor-linked highlighting */
.mlir-line{display:block;}

/* Lambda math display */
.lambda-math-panel{padding:6px 12px;background:var(--surface);border-top:1px solid var(--overlay);font-size:13px;min-height:28px;display:flex;align-items:center;gap:8px;flex-shrink:0;overflow-x:auto;}
.lambda-math-panel .math-label{font-size:10px;font-weight:600;color:var(--muted);text-transform:uppercase;letter-spacing:.5px;white-space:nowrap;}
.lambda-math-panel .math-content{color:var(--text);flex:1;min-width:0;}
.lambda-math-panel .math-content .katex{font-size:14px;}
.lambda-math-panel .math-placeholder{color:var(--muted);font-size:11px;font-style:italic;}
.mlir-line.cursor-match{background:rgba(249,226,175,0.22);outline:1px solid rgba(249,226,175,0.35);border-radius:2px;}
[data-theme="light"] .mlir-line.cursor-match{background:rgba(223,142,29,0.12);outline:1px solid rgba(223,142,29,0.25);}
.mlir-line.hl-deriv-line.cursor-match{background:rgba(203,166,247,0.28);outline-color:rgba(203,166,247,0.4);}
[data-theme="light"] .mlir-line.hl-deriv-line.cursor-match{background:rgba(136,57,239,0.16);outline-color:rgba(136,57,239,0.3);}
</style>
</head>
<body>

<header>
  <h1>∂SQL</h1>
  <span class="sep">—</span>
  <span class="subtitle">SQL &amp; MLIR Explorer</span>
  <div class="toolbar">
    <button class="btn-primary" onclick="runAll()" title="Ctrl+Enter">&#9654; Run</button>
    <button class="btn-secondary" onclick="compareMode()">Compare F/B</button>
    <button class="btn-secondary" id="theme-toggle" onclick="toggleTheme()" title="Toggle light/dark mode">&#9788; Light</button>
  </div>
</header>

<div class="grid">
  <!-- Top-left: SQL editor with tabs -->
  <div class="quadrant" id="q-sql">
    <div class="tabs-bar" id="sql-tabs-bar">
    </div>
    <div class="editor-wrap">
      <pre id="sql-highlight" aria-hidden="true"></pre>
      <textarea class="sql-editor" id="sql-editor" placeholder="Enter SQL query here...&#10;&#10;Ctrl+Enter to execute" spellcheck="false"></textarea>
    </div>
    <div class="lambda-math-panel" id="lambda-math-panel">
      <span class="math-label">&#955;</span>
      <span class="math-content" id="lambda-math-content"><span class="math-placeholder">No lambda expression detected</span></span>
    </div>
  </div>

  <!-- Top-right: Code viewer -->
  <div class="quadrant" id="q-code">
    <div class="code-toolbar">
      <select id="stage-select" onchange="onStageChange()">
        <option value="-1">— no pipeline loaded —</option>
      </select>
      <label><input type="checkbox" id="optimized-check" onchange="onOptimizedToggle()"> Optimized</label>
    </div>
    <div class="code-view" id="code-view-single">
      <pre class="code-output" id="code-output">Load a pipeline or use Compare to view IR.</pre>
    </div>
    <div class="compare-container" id="code-view-compare" style="display:none;">
      <div class="compare-pane">
        <div class="compare-label">Backward (derivateBackwards)</div>
        <pre class="code-output" id="compare-backward"></pre>
      </div>
      <div class="compare-pane">
        <div class="compare-label">Forward (derivateForwards)</div>
        <pre class="code-output" id="compare-forward"></pre>
      </div>
    </div>
  </div>

  <!-- Bottom-left: Results (table + plot) -->
  <div class="quadrant" id="q-results">
    <div class="panel-header">Results</div>
    <div class="results-split">
      <div class="results-table-pane" id="results-table-pane">
        <div class="results-container" id="results-container">
          <div class="info">Run a query to see results.</div>
        </div>
      </div>
      <div class="results-plot-pane" id="results-plot-pane">
        <svg id="results-plot" width="100%" height="100%"></svg>
      </div>
    </div>
  </div>

  <!-- Bottom-right: Timing -->
  <div class="quadrant" id="q-timing">
    <div class="panel-header">Timing</div>
    <div class="timing-container" id="timing-container">
      <div class="info">Execute a query to see timing breakdown.</div>
    </div>
  </div>
</div>

<div class="status-bar">
  <span id="status">Ready</span>
  <span id="status-timing">0 ms</span>
</div>

<script>
// ── State ──────────────────────────────────────────────────────────
let tabs = [
  {name:'LR (manual)', content:
`-- Linear Regression: learn y = 5x + 10 via manual gradient descent
with recursive gd (id, a, b, d_a, d_b) as (
   select 1,10::float,10::float,0::float,0::float
UNION ALL
   select id+1, a-0.5*avg(2*x*(a*x+b-y)), b-0.5*avg(2*(a*x+b-y)), avg(2*x*(a*x+b-y)), avg(2*(a*x+b-y)) from gd, data where id < 20 group by id,a,b)
select * from gd order by id;`},
  {name:'LR (forward)', content:
`-- Linear Regression with forward-mode automatic differentiation
with recursive gd (id, a, b, d_a, d_b) as (
   select 1,10::float,10::float,0::float,0::float
UNION ALL
   select id+1, a-0.5*avg(d_a), b-0.5*avg(d_b), avg(d_a), avg(d_b) from derivateForwards(TABLE (select id,a,b,x,y from gd, data where id < 20), lambda (x) ((x.a * x.x + x.b - x.y)^2)) group by id,a,b)
select * from gd order by id;`},
  {name:'LR (backward)', content:
`-- Linear Regression with backward-mode (reverse) automatic differentiation
with recursive gd (id, a, b, d_a, d_b) as (
   select 1,10::float,10::float,0::float,0::float
UNION ALL
   select id+1, a-0.5*avg(d_a), b-0.5*avg(d_b), avg(d_a), avg(d_b) from derivateBackwards(TABLE (select id,a,b,x,y from gd, data where id < 20), lambda (x) ((x.a * x.x + x.b - x.y)^2)) group by id,a,b)
select * from gd order by id;`},
  {name:'Iris NN (manual)', content:
`-- 2-layer Neural Network on Iris (manual backprop with arrays)
with recursive gd (id,w_xh,w_ho) as (
   select 0,
'{{-0.16595599,0.44064899,-0.99977125,-0.39533485,-0.70648822,-0.81532281,-0.62747958,-0.30887855,-0.20646505,0.07763347,-0.16161097,0.370439,-0.5910955,0.75623487,-0.94522481,0.34093502,-0.1653904,0.11737966,-0.71922612,-0.60379702}
,{0.60148914,0.93652315,-0.37315164,0.38464523,0.7527783,0.78921333,-0.82991158,-0.92189043,-0.66033916,0.75628501,-0.80330633,-0.15778475,0.91577906,0.06633057,0.38375423,-0.36896874,0.37300186,0.66925134,-0.96342345,0.50028863}
,{0.97772218,0.49633131,-0.43911202,0.57855866,-0.79354799,-0.10421295,0.81719101,-0.4127717,-0.42444932,-0.73994286,-0.96126608,0.35767107,-0.57674377,-0.46890668,-0.01685368,-0.89327491,0.14823521,-0.70654285,0.17861107,0.39951672}
,{-0.79533114,-0.17188802,0.38880032,-0.17164146,-0.90009308,0.07179281,0.32758929,0.02977822,0.88918951,0.17311008,0.80680383,-0.72505059,-0.72144731,0.61478258,-0.20464633,-0.66929161,0.85501716,-0.30446828,0.50162421,0.45199597}}'::float[],
'{{0.76661218,0.24734441,0.50188487},{-0.30220332,-0.46014422,0.79177244},{-0.14381762,0.92968009,0.326883},{0.24339144,-0.77050805,0.89897852},{-0.10017573,0.15677923,-0.18372639},{-0.52594604,0.80675904,0.14735897}
,{-0.99425935,0.23428983,-0.3467102},{0.0541162,0.7718842,-0.28546048},{0.8170703,0.24672023,-0.96835751},{0.85887447,0.38179384,0.9946457},{-0.65531898,-0.7257285,0.86519093},{0.39363632,-0.86799965,0.51092611}
,{0.50775238,0.84604907,0.42304952},{-0.75145808,-0.96023973,-0.94757803},{-0.94338702,-0.50757786,0.7200559},{0.07766213,0.10564396,0.68406178},{-0.75165337,-0.44163264,0.17151854},{0.9391915,0.12206044,-0.96270542},{0.60126535,-0.53405145,0.61421039}
,{-0.22427871,0.72708371,0.49424329}}'::float[]
union all
   select id+1, w_xh - 0.01 * sum(transpose(img)**d_xh), w_ho - 0.01 * sum(transpose(a_xh)**d_ho)
   from (
      select l_xh * a_xh * (1-a_xh) as d_xh, *
      from (
         select d_ho**transpose(w_ho) as l_xh, *
         from (
            select (l_ho * a_ho * (1-a_ho)) as d_ho, *
            from (
               select 2*(a_ho-one_hot) as l_ho, *
               from (
                  select sig(a_xh**w_ho) as a_ho, *
                  from (
                     select sig(img**w_xh) as a_xh, *
                     from (select * from iris3) tmp, gd where id < 20 ) tmp
               ) tmp
            ) tmp
         ) tmp
      ) tmp
   ) tmp
   group by id, w_ho, w_xh
)
, test as (select id, correct, count(*) from (select id, highestposition(sig(sig(img**w_xh)**w_ho))=highestposition(one_hot) as correct from iris3,gd) tmp group by id, correct)
select id as iter, count*1.0/(select sum(count) from test t2 where t1.id=t2.id) as accuracy from test t1 where correct=true order by id;`},
  {name:'Iris NN (backward)', content:
`-- 2-layer Neural Network on Iris with backward-mode (reverse) automatic differentiation
with recursive gd (id,w_xh,w_ho) as (
   select 0,
'{{-0.16595599,0.44064899,-0.99977125,-0.39533485,-0.70648822,-0.81532281,-0.62747958,-0.30887855,-0.20646505,0.07763347,-0.16161097,0.370439,-0.5910955,0.75623487,-0.94522481,0.34093502,-0.1653904,0.11737966,-0.71922612,-0.60379702}
,{0.60148914,0.93652315,-0.37315164,0.38464523,0.7527783,0.78921333,-0.82991158,-0.92189043,-0.66033916,0.75628501,-0.80330633,-0.15778475,0.91577906,0.06633057,0.38375423,-0.36896874,0.37300186,0.66925134,-0.96342345,0.50028863}
,{0.97772218,0.49633131,-0.43911202,0.57855866,-0.79354799,-0.10421295,0.81719101,-0.4127717,-0.42444932,-0.73994286,-0.96126608,0.35767107,-0.57674377,-0.46890668,-0.01685368,-0.89327491,0.14823521,-0.70654285,0.17861107,0.39951672}
,{-0.79533114,-0.17188802,0.38880032,-0.17164146,-0.90009308,0.07179281,0.32758929,0.02977822,0.88918951,0.17311008,0.80680383,-0.72505059,-0.72144731,0.61478258,-0.20464633,-0.66929161,0.85501716,-0.30446828,0.50162421,0.45199597}}'::float[],
'{{0.76661218,0.24734441,0.50188487},{-0.30220332,-0.46014422,0.79177244},{-0.14381762,0.92968009,0.326883},{0.24339144,-0.77050805,0.89897852},{-0.10017573,0.15677923,-0.18372639},{-0.52594604,0.80675904,0.14735897}
,{-0.99425935,0.23428983,-0.3467102},{0.0541162,0.7718842,-0.28546048},{0.8170703,0.24672023,-0.96835751},{0.85887447,0.38179384,0.9946457},{-0.65531898,-0.7257285,0.86519093},{0.39363632,-0.86799965,0.51092611}
,{0.50775238,0.84604907,0.42304952},{-0.75145808,-0.96023973,-0.94757803},{-0.94338702,-0.50757786,0.7200559},{0.07766213,0.10564396,0.68406178},{-0.75165337,-0.44163264,0.17151854},{0.9391915,0.12206044,-0.96270542},{0.60126535,-0.53405145,0.61421039}
,{-0.22427871,0.72708371,0.49424329}}'::float[]
union all
   select id+1, w_xh - 0.01 * transpose(sum(d_w_xh)), w_ho - 0.01 * transpose(sum(d_w_ho))
   from  derivateBackwards(TABLE(select * from (select * from iris3) tmp,gd where id < 20),lambda(x)(( sig(sig(x.img**x.w_xh)**x.w_ho) - one_hot)^2 ))
   group by id, w_ho, w_xh
)
, test as (select id, correct, count(*) from (select id, highestposition(sig(sig(img**w_xh)**w_ho))=highestposition(one_hot) as correct from iris3,gd) tmp group by id, correct)
select id as iter, count*1.0/(select sum(count) from test t2 where t1.id=t2.id) as accuracy from test t1 where correct=true order by id;`}
];
let activeTab = 0;
let pipelineStages = null; // array of {name, mlir}
let comparePipelineB = null; // backward pipeline stages in compare mode
let comparePipelineF = null; // forward pipeline stages in compare mode
let inCompareMode = false;

const editorEl    = document.getElementById('sql-editor');
const highlightEl = document.getElementById('sql-highlight');
const statusEl    = document.getElementById('status');
const timingEl    = document.getElementById('status-timing');
const stageSelEl  = document.getElementById('stage-select');
const optCheck    = document.getElementById('optimized-check');

// ── SQL Syntax Highlighting ────────────────────────────────────────
const SQL_KW = /\b(SELECT|FROM|WHERE|WITH|RECURSIVE|UNION|ALL|GROUP|BY|ORDER|AS|JOIN|LEFT|RIGHT|INNER|OUTER|ON|AND|OR|NOT|IN|EXISTS|BETWEEN|LIKE|CASE|WHEN|THEN|ELSE|END|INSERT|INTO|VALUES|CREATE|TABLE|IF|LIMIT|HAVING|DISTINCT|DESC|ASC|COPY|HEADER|DELIMITER|GENERATE_SERIES|TRUE|FALSE|NULL|IS)\b/gi;
const SQL_FN = /\b(derivateForwards|derivateBackwards|umbra\.derivation|lambda|sig|transpose|sqrt|exp|log|sin|cos|array_agg|array_fill|array|sum|avg|count|min|max|highestposition|random|row_number)\b/gi;
const SQL_TYPE = /\b(float|int|integer|bigint|text|boolean|varchar|double|decimal)\b/gi;

function highlightSQL(code){
  let s = escapeHtml(code);
  // Comments (-- to end of line)
  s = s.replace(/(--[^\n]*)/g, '<span class="sql-comment">$1</span>');
  // String literals
  s = s.replace(/('(?:[^'\\]|\\.)*')/g, '<span class="sql-str">$1</span>');
  // Types (must come before keywords for float[], etc.)
  s = s.replace(SQL_TYPE, '<span class="sql-type">$&</span>');
  // Functions (before keywords so 'derivateForwards' isn't split)
  s = s.replace(SQL_FN, '<span class="sql-fn">$&</span>');
  // Keywords
  s = s.replace(SQL_KW, m => '<span class="sql-kw">' + m + '</span>');
  // Numbers
  s = s.replace(/\b(\d+\.?\d*(?:e[+-]?\d+)?)\b/gi, '<span class="sql-num">$1</span>');
  // Operators
  s = s.replace(/(\*\*|::|\|\||&lt;=|&gt;=|&lt;&gt;|!=)/g, '<span class="sql-op">$1</span>');
  // Append newline so pre always matches textarea scrollHeight
  return s + '\n';
}

function syncHighlight(){
  highlightEl.innerHTML = highlightSQL(editorEl.value);
  highlightEl.scrollTop = editorEl.scrollTop;
  highlightEl.scrollLeft = editorEl.scrollLeft;
  updateLambdaMath(editorEl.value);
}

editorEl.addEventListener('input', syncHighlight);
editorEl.addEventListener('scroll', () => {
  highlightEl.scrollTop = editorEl.scrollTop;
  highlightEl.scrollLeft = editorEl.scrollLeft;
});

// ── Lambda → LaTeX rendering ──────────────────────────────────────
const lambdaMathEl = document.getElementById('lambda-math-content');

function extractLambdaBody(sql) {
  // Find LAMBDA (param) (body) — case-insensitive
  const m = sql.match(/lambda\s*\(\s*(\w+)\s*\)\s*\(/i);
  if (!m) return null;
  const param = m[1];
  const start = m.index + m[0].length;
  // Match balanced parens to find body end
  let depth = 1, i = start;
  while (i < sql.length && depth > 0) {
    if (sql[i] === '(') depth++;
    else if (sql[i] === ')') depth--;
    i++;
  }
  if (depth !== 0) return null;
  return { param, body: sql.substring(start, i - 1).trim() };
}

// Tokenizer for lambda expressions
function tokenizeLambda(s) {
  const tokens = [];
  let i = 0;
  while (i < s.length) {
    if (/\s/.test(s[i])) { i++; continue; }
    // Two-char operators
    if (s[i] === '*' && s[i+1] === '*') { tokens.push({type:'op',val:'**'}); i+=2; continue; }
    // Single-char operators and parens
    if ('+-*/^(),'.includes(s[i])) { tokens.push({type:'op',val:s[i]}); i++; continue; }
    // Numbers
    if (/[0-9.]/.test(s[i])) {
      let n = '';
      while (i < s.length && /[0-9.eE+-]/.test(s[i]) && !(s[i]==='-' && n.length>0 && !/[eE]$/.test(n)))
        { n += s[i]; i++; }
      tokens.push({type:'num',val:n}); continue;
    }
    // Identifiers (x.col or func name)
    if (/[a-zA-Z_]/.test(s[i])) {
      let id = '';
      while (i < s.length && /[a-zA-Z0-9_.]/.test(s[i])) { id += s[i]; i++; }
      tokens.push({type:'id',val:id}); continue;
    }
    i++; // skip unknown
  }
  return tokens;
}

// Recursive descent parser: expr → term ((+|-) term)*
// term → factor ((*|/|**) factor)* , pow → unary (^ unary)*
function parseLambda(tokens, param) {
  let pos = 0;
  function peek() { return pos < tokens.length ? tokens[pos] : null; }
  function consume() { return tokens[pos++]; }

  function parseExpr() {
    let left = parseTerm();
    while (peek() && peek().type==='op' && (peek().val==='+' || peek().val==='-')) {
      const op = consume().val;
      const right = parseTerm();
      left = {type:'binop', op, left, right};
    }
    return left;
  }

  function parseTerm() {
    let left = parsePow();
    while (peek() && peek().type==='op' && (peek().val==='*' || peek().val==='/' || peek().val==='**')) {
      const op = consume().val;
      const right = parsePow();
      left = {type:'binop', op, left, right};
    }
    return left;
  }

  function parsePow() {
    let base = parseUnary();
    if (peek() && peek().type==='op' && peek().val==='^') {
      consume();
      const exp = parseUnary();
      return {type:'binop', op:'^', left:base, right:exp};
    }
    return base;
  }

  function parseUnary() {
    if (peek() && peek().type==='op' && peek().val==='-') {
      consume();
      return {type:'neg', child: parseUnary()};
    }
    return parseAtom();
  }

  function parseAtom() {
    const t = peek();
    if (!t) return {type:'num', val:'?'};

    // Parenthesized expression
    if (t.type==='op' && t.val==='(') {
      consume();
      const inner = parseExpr();
      if (peek() && peek().val===')') consume();
      return inner;
    }

    // Number
    if (t.type==='num') { consume(); return {type:'num', val:t.val}; }

    // Identifier: function call or column reference
    if (t.type==='id') {
      consume();
      // Function call: name(args)
      if (peek() && peek().val==='(') {
        consume(); // (
        const args = [];
        if (!(peek() && peek().val===')')) {
          args.push(parseExpr());
          while (peek() && peek().val===',') { consume(); args.push(parseExpr()); }
        }
        if (peek() && peek().val===')') consume();
        return {type:'call', name:t.val, args};
      }
      // Column ref: param.col → just col
      const name = t.val.startsWith(param+'.') ? t.val.substring(param.length+1) : t.val;
      return {type:'var', name};
    }

    consume();
    return {type:'num', val:'?'};
  }

  return parseExpr();
}

// AST → LaTeX string
function astToLatex(node) {
  if (!node) return '?';
  switch (node.type) {
    case 'num': return node.val;
    case 'var': {
      const n = node.name;
      if (/^w_/.test(n)) return '\\mathbf{W}_{' + n.substring(2).replace(/_/g,'') + '}';
      if (n === 'one_hot') return '\\mathbf{y}';
      if (n === 'img') return '\\mathbf{x}';
      if (/^(w_|img|one_hot)/.test(n)) return '\\mathbf{' + n.replace(/_/g, '\\_') + '}';
      return n.length === 1 ? n : '\\text{' + n.replace(/_/g, '\\_') + '}';
    }
    case 'neg': return '-' + astToLatex(node.child);
    case 'binop': {
      const l = astToLatex(node.left);
      const r = astToLatex(node.right);
      switch (node.op) {
        case '+': return l + ' + ' + r;
        case '-': return l + ' - ' + r;
        case '*': {
          const lp = needsParens(node.left, '*') ? '\\left(' + l + '\\right)' : l;
          const rp = needsParens(node.right, '*') ? '\\left(' + r + '\\right)' : r;
          return lp + ' \\cdot ' + rp;
        }
        case '/': return '\\frac{' + l + '}{' + r + '}';
        case '^': {
          const base = needsParens(node.left, '^') ? '\\left(' + l + '\\right)' : l;
          return base + '^{' + r + '}';
        }
        case '**': return l + ' \\cdot ' + r;
      }
      return '?';
    }
    case 'call': {
      const fname = node.name.toLowerCase();
      const a = node.args.map(x => astToLatex(x));
      switch (fname) {
        case 'sig': case 'sigmoid':
          return '\\sigma\\!\\left(' + a.join(', ') + '\\right)';
        case 'exp': return 'e^{' + a[0] + '}';
        case 'log': case 'ln': return '\\ln\\!\\left(' + a[0] + '\\right)';
        case 'sqrt': return '\\sqrt{' + a[0] + '}';
        case 'sin': return '\\sin\\!\\left(' + a[0] + '\\right)';
        case 'cos': return '\\cos\\!\\left(' + a[0] + '\\right)';
        case 'transpose': return a[0] + '^{\\top}';
        case 'abs': return '\\left|' + a[0] + '\\right|';
        default: return '\\text{' + fname + '}(' + a.join(', ') + ')';
      }
    }
    default: return '?';
  }
}

function needsParens(node, parentOp) {
  if (!node || node.type !== 'binop') return false;
  if (parentOp === '^') return node.op === '+' || node.op === '-' || node.op === '*' || node.op === '/';
  if (parentOp === '*') return node.op === '+' || node.op === '-';
  return false;
}

function updateLambdaMath(sql) {
  if (typeof katex === 'undefined') {
    // KaTeX not loaded yet — retry once after a short delay
    setTimeout(() => updateLambdaMath(sql), 500);
    return;
  }
  const result = extractLambdaBody(sql);
  if (!result) {
    lambdaMathEl.innerHTML = '<span class="math-placeholder">No lambda expression detected</span>';
    return;
  }
  try {
    const tokens = tokenizeLambda(result.body);
    const ast = parseLambda(tokens, result.param);
    const latex = '\\mathcal{L} = ' + astToLatex(ast);
    katex.render(latex, lambdaMathEl, { throwOnError: false, displayMode: false });
  } catch(e) {
    lambdaMathEl.innerHTML = '<span class="math-placeholder">Could not parse lambda</span>';
  }
}

// ── Cursor-linked MLIR highlighting ───────────────────────────────
const SQL_KW_SET = new Set(['select','from','where','group','by','order','as','and','or','not',
  'join','on','left','right','inner','outer','cross','union','all','insert','into','values',
  'create','table','with','recursive','having','in','exists','between','like','case','when',
  'then','else','end','distinct','limit','offset','asc','desc','true','false','null']);

function getTokenAtCursor(){
  const pos = editorEl.selectionStart;
  const text = editorEl.value;
  if(!text || pos < 0 || pos > text.length) return null;
  // Check for ** operator (matrix multiply)
  if(pos > 0 && text.substring(pos-1,pos+1)==='**') return '**';
  if(pos < text.length-1 && text.substring(pos,pos+2)==='**') return '**';
  // Check single-char operators
  const ch = text[pos-1]||'';
  const chR = text[pos]||'';
  if('+-/'.includes(ch) && ch) return ch;
  if('+-/'.includes(chR) && chR) return chR;
  if(ch==='^') return '^';
  if(chR==='^') return '^';
  if(ch==='*' && text[pos]!=='*' && (pos<2||text[pos-2]!=='*')) return '*';
  if(chR==='*' && text[pos+1]!=='*' && (pos<1||text[pos-1]!=='*')) return '*';
  // Extract word (including dots for x.col references and underscores)
  let start=pos, end=pos;
  while(start>0 && /[\w.]/.test(text[start-1])) start--;
  while(end<text.length && /[\w.]/.test(text[end])) end++;
  const word = text.substring(start,end);
  if(!word) return null;
  return word;
}

function sqlTokenToMLIRPatterns(token){
  if(!token) return [];
  const t = token.toLowerCase();
  // Column references like x.a or just a
  if(/^\w+\.\w+$/.test(token)){
    const col = token.split('.')[1];
    return ['@\\w+::@'+col+'\\b', '@\\w+::@d_'+col+'\\b'];
  }
  // d_xxx derivative columns
  if(/^d_\w+$/.test(token)) return ['@\\w+::@'+token+'\\b'];
  // Operators
  if(token==='**') return ['ArrayMMul'];
  if(token==='*') return ['db\\.mul\\b'];
  if(token==='+') return ['db\\.add\\b'];
  if(token==='-') return ['db\\.sub\\b'];
  if(token==='/') return ['db\\.div\\b'];
  if(token==='^') return ['"PowerFloat"'];
  // Functions
  if(t==='sig') return ['"Sig"'];
  if(t==='transpose') return ['"ArrayTranspose"'];
  if(t==='log') return ['"Log"'];
  if(t==='sqrt') return ['"Sqrt"'];
  if(t==='exp') return ['"Exp"'];
  if(t==='sum'||t==='avg'||t==='count'||t==='min'||t==='max') return ['relalg\\.aggr'];
  if(t==='derivatebackwards'||t==='derivateforwards'||t==='umbra.derivation') return ['@\\w+::@d_\\w+'];
  // Column name (not a keyword)
  if(/^\w+$/.test(token) && !SQL_KW_SET.has(t)) return ['@\\w+::@'+token+'\\b', '@\\w+::@d_'+token+'\\b'];
  return [];
}

function cursorHighlightMLIR(){
  const lines = document.querySelectorAll('.mlir-line');
  if(!lines.length) return;
  const token = getTokenAtCursor();
  const patterns = sqlTokenToMLIRPatterns(token);
  if(!patterns.length){
    lines.forEach(el => el.classList.remove('cursor-match'));
    return;
  }
  const regexes = patterns.map(p => new RegExp(p,'i'));
  lines.forEach(el => {
    const txt = el.getAttribute('data-raw') || el.textContent;
    const match = regexes.some(r => r.test(txt));
    el.classList.toggle('cursor-match', match);
  });
}

editorEl.addEventListener('click', cursorHighlightMLIR);
editorEl.addEventListener('keyup', (e) => {
  if(e.key.startsWith('Arrow')||e.key==='Home'||e.key==='End') cursorHighlightMLIR();
});

// ── Tab Management ─────────────────────────────────────────────────
function renderTabs(){
  const bar = document.getElementById('sql-tabs-bar');
  bar.innerHTML = '';
  tabs.forEach((t,i) => {
    const d = document.createElement('div');
    d.className = 'sql-tab' + (i===activeTab?' active':'');
    d.setAttribute('data-idx', i);
    d.onclick = (e) => { if(!e.target.classList.contains('close-tab')) switchSqlTab(i); };
    let inner = escapeHtml(t.name);
    if(tabs.length > 1) inner += '<span class="close-tab" onclick="removeTab('+i+')">&#10005;</span>';
    d.innerHTML = inner;
    bar.appendChild(d);
  });
  const btn = document.createElement('button');
  btn.className = 'add-tab';
  btn.textContent = '+';
  btn.title = 'New tab';
  btn.onclick = addTab;
  bar.appendChild(btn);
}

function addTab(){
  tabs[activeTab].content = editorEl.value;
  const n = tabs.length + 1;
  tabs.push({name:'Query '+n, content:''});
  activeTab = tabs.length - 1;
  editorEl.value = '';
  renderTabs();
  editorEl.focus();
}

function switchSqlTab(idx){
  tabs[activeTab].content = editorEl.value;
  activeTab = idx;
  editorEl.value = tabs[idx].content;
  renderTabs();
  syncHighlight();
  editorEl.focus();
}

function removeTab(idx){
  if(tabs.length <= 1) return;
  tabs.splice(idx, 1);
  if(activeTab >= tabs.length) activeTab = tabs.length - 1;
  editorEl.value = tabs[activeTab].content;
  renderTabs();
  syncHighlight();
}

// ── Keyboard Shortcuts ─────────────────────────────────────────────
editorEl.addEventListener('keydown', (e) => {
  if(e.ctrlKey && e.key === 'Enter'){ e.preventDefault(); runAll(); }
  // Tab key inserts spaces
  if(e.key === 'Tab'){
    e.preventDefault();
    const s = editorEl.selectionStart, end = editorEl.selectionEnd;
    editorEl.value = editorEl.value.substring(0,s) + '  ' + editorEl.value.substring(end);
    editorEl.selectionStart = editorEl.selectionEnd = s + 2;
  }
});

// ── Run All (Execute + Pipeline in parallel) ──────────────────────
async function runAll(){
  exitCompareMode();
  tabs[activeTab].content = editorEl.value;
  const sql = editorEl.value.trim();
  if(!sql) return;
  statusEl.textContent = 'Running…';
  timingEl.textContent = '';
  document.getElementById('results-container').innerHTML = '<div class="loading">Executing query…</div>';
  document.getElementById('timing-container').innerHTML = '<div class="loading">Waiting for results…</div>';
  document.getElementById('code-output').textContent = 'Loading pipeline…';

  try {
    const t0 = performance.now();
    const [execResp, pipeResp] = await Promise.all([
      fetch('/api/execute', {method:'POST', headers:{'Content-Type':'application/json'}, body:JSON.stringify({sql})}),
      fetch('/api/pipeline', {method:'POST', headers:{'Content-Type':'application/json'}, body:JSON.stringify({sql})})
    ]);
    const data = await execResp.json();
    const pipeData = await pipeResp.json();
    const wall = (performance.now()-t0).toFixed(1);

    // ── Results ──
    if(data.error){
      document.getElementById('results-container').innerHTML = '<div class="error">'+escapeHtml(data.error)+'</div>';
      document.getElementById('timing-container').innerHTML = '';
      renderPlot(null, null);
      statusEl.textContent = 'Error';
      timingEl.textContent = wall+' ms (wall)';
    } else if(data.columns && data.columns.length > 0){
      let h = '<table class="results-table"><thead><tr>';
      data.columns.forEach(c => h += '<th>'+escapeHtml(c)+'</th>');
      h += '</tr></thead><tbody>';
      data.rows.forEach(row => {
        h += '<tr>';
        row.forEach(v => h += '<td>'+escapeHtml(v)+'</td>');
        h += '</tr>';
      });
      h += '</tbody></table>';
      document.getElementById('results-container').innerHTML = h;
      statusEl.textContent = data.rows.length + ' row(s)';
      renderPlot(data.columns, data.rows);
      timingEl.textContent = wall + ' ms (wall)';
      if(data.timing && Object.keys(data.timing).length > 0) renderTiming(data.timing);
      else document.getElementById('timing-container').innerHTML = '<div class="info">No timing data.</div>';
    } else {
      document.getElementById('results-container').innerHTML = '<div class="info">Statement executed. No rows returned.</div>';
      statusEl.textContent = 'OK';
      renderPlot(null, null);
      timingEl.textContent = wall + ' ms (wall)';
    }

    // ── Pipeline stages ──
    if(!pipeData.error){
      pipelineStages = pipeData.stages || [];
      populateStageDropdown();
      if(pipelineStages.length > 0){
        const idx = (optCheck.checked && pipelineStages.length > 1) ? 1 : 0;
        stageSelEl.value = idx;
        showStage(idx);
      }
      if(!data.error) statusEl.textContent += ' — ' + pipelineStages.length + ' stage(s)';
    } else {
      document.getElementById('code-output').textContent = pipeData.error;
    }
  } catch(err){
    document.getElementById('results-container').innerHTML = '<div class="error">'+escapeHtml(err.message)+'</div>';
    statusEl.textContent = 'Error';
  }
}

// ── Timing Visualization ───────────────────────────────────────────
const PHASE_ORDER = ['QOpt','lowerRelAlg','lowerSubOp','lowerDB','lowerDSA','lowerToLLVM','toLLVMIR','llvmOptimize','llvmCodeGen','executionTime'];
const PHASE_COLORS = {
  QOpt:'var(--blue)', lowerRelAlg:'var(--teal)', lowerSubOp:'var(--green)',
  lowerDB:'var(--yellow)', lowerDSA:'var(--peach)', lowerToLLVM:'var(--mauve)',
  toLLVMIR:'var(--pink)', llvmOptimize:'var(--red)', llvmCodeGen:'#fab387',
  executionTime:'var(--green)'
};

function renderTiming(timing){
  // Sort by PHASE_ORDER, then append any unknown keys
  const keys = [];
  PHASE_ORDER.forEach(k => { if(timing[k] !== undefined) keys.push(k); });
  Object.keys(timing).forEach(k => { if(!keys.includes(k)) keys.push(k); });

  const maxVal = Math.max(...keys.map(k => timing[k]), 0.001);
  let html = '';
  keys.forEach(k => {
    const ms = timing[k];
    const pct = Math.max((ms / maxVal) * 100, 0.5);
    const color = PHASE_COLORS[k] || 'var(--subtext)';
    html += '<div class="timing-bar-group">';
    html += '<div class="timing-label"><span>'+escapeHtml(k)+'</span><span>'+ms.toFixed(2)+' ms</span></div>';
    html += '<div class="timing-bar-track"><div class="timing-bar-fill" style="width:'+pct+'%;background:'+color+';"></div></div>';
    html += '</div>';
  });

  // Summary table
  const total = keys.reduce((s,k) => s + timing[k], 0);
  html += '<div class="timing-summary"><table>';
  keys.forEach(k => {
    html += '<tr><td>'+escapeHtml(k)+'</td><td>'+timing[k].toFixed(2)+' ms</td></tr>';
  });
  html += '<tr style="border-top:1px solid var(--overlay);font-weight:600;"><td>Total</td><td>'+total.toFixed(2)+' ms</td></tr>';
  html += '</table></div>';

  document.getElementById('timing-container').innerHTML = html;
}

// ── Plot Visualization ────────────────────────────────────────────
function renderPlot(columns, rows) {
  const svg = document.getElementById('results-plot');
  if(!svg) return;
  const ns = 'http://www.w3.org/2000/svg';
  svg.innerHTML = '';
  if(!columns || columns.length < 2 || !rows || rows.length === 0) {
    const t = document.createElementNS(ns, 'text');
    t.setAttribute('x', '50%'); t.setAttribute('y', '50%');
    t.setAttribute('class', 'plot-empty');
    t.textContent = 'Need ≥ 2 numeric columns to plot';
    svg.appendChild(t);
    return;
  }
  // Extract x (col 0) and y (col 1) as numbers
  const xVals = rows.map(r => parseFloat(r[0]));
  const yVals = rows.map(r => parseFloat(r[1]));
  if(xVals.some(isNaN) || yVals.some(isNaN)) {
    const t = document.createElementNS(ns, 'text');
    t.setAttribute('x', '50%'); t.setAttribute('y', '50%');
    t.setAttribute('class', 'plot-empty');
    t.textContent = 'First two columns must be numeric';
    svg.appendChild(t);
    return;
  }

  const rect = svg.getBoundingClientRect();
  const W = rect.width || 300, H = rect.height || 200;
  svg.setAttribute('viewBox', `0 0 ${W} ${H}`);

  const margin = {top:20, right:20, bottom:40, left:55};
  const pw = W - margin.left - margin.right;
  const ph = H - margin.top - margin.bottom;

  const xMin = Math.min(...xVals), xMax = Math.max(...xVals);
  const yMin = Math.min(...yVals), yMax = Math.max(...yVals);
  const xRange = xMax - xMin || 1;
  const yRange = yMax - yMin || 1;
  const xPad = xRange * 0.05, yPad = yRange * 0.08;

  function sx(v){ return margin.left + ((v - (xMin - xPad)) / (xRange + 2*xPad)) * pw; }
  function sy(v){ return margin.top + ph - ((v - (yMin - yPad)) / (yRange + 2*yPad)) * ph; }

  const g = document.createElementNS(ns, 'g');

  // Grid + ticks
  const nTicksX = Math.min(Math.max(rows.length, 5), 10);
  const nTicksY = 5;
  for(let i = 0; i <= nTicksY; i++) {
    const v = yMin + (yRange * i / nTicksY);
    const y = sy(v);
    const line = document.createElementNS(ns, 'line');
    line.setAttribute('x1', margin.left); line.setAttribute('x2', W - margin.right);
    line.setAttribute('y1', y); line.setAttribute('y2', y);
    line.setAttribute('class', 'plot-grid');
    g.appendChild(line);
    const txt = document.createElementNS(ns, 'text');
    txt.setAttribute('x', margin.left - 5); txt.setAttribute('y', y + 3);
    txt.setAttribute('text-anchor', 'end');
    txt.setAttribute('class', 'plot-tick');
    txt.textContent = v.toPrecision(3);
    g.appendChild(txt);
  }
  for(let i = 0; i <= nTicksX && i < rows.length; i++) {
    const idx = Math.round(i * (rows.length - 1) / Math.min(nTicksX, rows.length - 1));
    if(idx >= rows.length) continue;
    const v = xVals[idx];
    const x = sx(v);
    const txt = document.createElementNS(ns, 'text');
    txt.setAttribute('x', x); txt.setAttribute('y', H - margin.bottom + 15);
    txt.setAttribute('text-anchor', 'middle');
    txt.setAttribute('class', 'plot-tick');
    txt.textContent = Number.isInteger(v) ? v : v.toPrecision(3);
    g.appendChild(txt);
  }

  // Axes
  const xAxis = document.createElementNS(ns, 'line');
  xAxis.setAttribute('x1', margin.left); xAxis.setAttribute('x2', W - margin.right);
  xAxis.setAttribute('y1', margin.top + ph); xAxis.setAttribute('y2', margin.top + ph);
  xAxis.setAttribute('class', 'plot-axis');
  g.appendChild(xAxis);
  const yAxis = document.createElementNS(ns, 'line');
  yAxis.setAttribute('x1', margin.left); yAxis.setAttribute('x2', margin.left);
  yAxis.setAttribute('y1', margin.top); yAxis.setAttribute('y2', margin.top + ph);
  yAxis.setAttribute('class', 'plot-axis');
  g.appendChild(yAxis);

  // Axis labels
  const xLabel = document.createElementNS(ns, 'text');
  xLabel.setAttribute('x', margin.left + pw / 2); xLabel.setAttribute('y', H - 5);
  xLabel.setAttribute('text-anchor', 'middle');
  xLabel.setAttribute('class', 'plot-label');
  xLabel.textContent = columns[0];
  g.appendChild(xLabel);
  const yLabel = document.createElementNS(ns, 'text');
  yLabel.setAttribute('transform', `translate(12,${margin.top + ph / 2}) rotate(-90)`);
  yLabel.setAttribute('text-anchor', 'middle');
  yLabel.setAttribute('class', 'plot-label');
  yLabel.textContent = columns[1];
  g.appendChild(yLabel);

  // Line
  if(rows.length > 1) {
    let pathD = '';
    for(let i = 0; i < rows.length; i++) {
      pathD += (i === 0 ? 'M' : 'L') + sx(xVals[i]).toFixed(1) + ',' + sy(yVals[i]).toFixed(1);
    }
    const path = document.createElementNS(ns, 'path');
    path.setAttribute('d', pathD);
    path.setAttribute('class', 'plot-line');
    g.appendChild(path);
  }

  // Dots
  for(let i = 0; i < rows.length; i++) {
    const c = document.createElementNS(ns, 'circle');
    c.setAttribute('cx', sx(xVals[i]).toFixed(1));
    c.setAttribute('cy', sy(yVals[i]).toFixed(1));
    c.setAttribute('r', rows.length > 50 ? 2 : 3.5);
    c.setAttribute('class', 'plot-dot');
    g.appendChild(c);
  }

  svg.appendChild(g);
}

// ── Pipeline Stages ────────────────────────────────────────────────
function populateStageDropdown(){
  stageSelEl.innerHTML = '';
  if(!pipelineStages || pipelineStages.length === 0){
    stageSelEl.innerHTML = '<option value="-1">— no pipeline loaded —</option>';
    return;
  }
  pipelineStages.forEach((s,i) => {
    const opt = document.createElement('option');
    opt.value = i;
    opt.textContent = s.name;
    stageSelEl.appendChild(opt);
  });
}

function showStage(idx){
  if(!pipelineStages || idx < 0 || idx >= pipelineStages.length) return;
  document.getElementById('code-output').innerHTML = highlightMLIR(pipelineStages[idx].mlir);
}

function onStageChange(){
  const idx = parseInt(stageSelEl.value);
  if(idx < 0) return;
  if(inCompareMode) showCompareStage(idx);
  else showStage(idx);
}

function onOptimizedToggle(){
  const stages = inCompareMode ? (comparePipelineB || comparePipelineF || []) : pipelineStages;
  if(!stages || stages.length < 2) return;
  const idx = optCheck.checked ? 1 : 0;
  stageSelEl.value = idx;
  if(inCompareMode) showCompareStage(idx);
  else showStage(idx);
}

// ── Compare Mode (Forward / Backward) ─────────────────────────────
async function compareMode(){
  tabs[activeTab].content = editorEl.value;
  const sql = editorEl.value.trim();
  if(!sql) return;

  inCompareMode = true;
  document.getElementById('code-view-single').style.display = 'none';
  document.getElementById('code-view-compare').style.display = 'flex';
  document.getElementById('compare-backward').textContent = 'Generating backward…';
  document.getElementById('compare-forward').textContent = 'Generating forward…';
  document.getElementById('timing-container').innerHTML = '<div class="loading">Running forward & backward for timing comparison…</div>';
  statusEl.textContent = 'Comparing derivation modes…';

  const backwardSql = sql.replace(/derivateForwards/gi, 'derivateBackwards').replace(/umbra\.derivation/gi, 'derivateBackwards');
  const forwardSql  = sql.replace(/derivateBackwards/gi, 'derivateForwards').replace(/umbra\.derivation/gi, 'derivateForwards');

  try {
    // Load pipelines + execute both in parallel
    const [bPipe, fPipe, bExec, fExec] = await Promise.all([
      fetch('/api/pipeline', {method:'POST', headers:{'Content-Type':'application/json'}, body:JSON.stringify({sql:backwardSql})}),
      fetch('/api/pipeline', {method:'POST', headers:{'Content-Type':'application/json'}, body:JSON.stringify({sql:forwardSql})}),
      fetch('/api/execute', {method:'POST', headers:{'Content-Type':'application/json'}, body:JSON.stringify({sql:backwardSql})}),
      fetch('/api/execute', {method:'POST', headers:{'Content-Type':'application/json'}, body:JSON.stringify({sql:forwardSql})})
    ]);
    const bPipeData = await bPipe.json();
    const fPipeData = await fPipe.json();
    const bExecData = await bExec.json();
    const fExecData = await fExec.json();

    // Store pipeline stages for both modes
    comparePipelineB = bPipeData.stages || [];
    comparePipelineF = fPipeData.stages || [];

    // Populate stage dropdown from backward pipeline (both should have same stage names)
    const stages = comparePipelineB.length > 0 ? comparePipelineB : comparePipelineF;
    stageSelEl.innerHTML = '';
    if(stages.length === 0){
      stageSelEl.innerHTML = '<option value="-1">— no pipeline loaded —</option>';
    } else {
      stages.forEach((s,i) => {
        const opt = document.createElement('option');
        opt.value = i;
        opt.textContent = s.name;
        stageSelEl.appendChild(opt);
      });
    }

    // Show initial stage (optimized if checked, else canonical)
    const idx = (optCheck.checked && stages.length > 1) ? 1 : 0;
    stageSelEl.value = idx;
    showCompareStage(idx);

    // Show timing comparison
    const bTiming = bExecData.timing || {};
    const fTiming = fExecData.timing || {};
    if(Object.keys(bTiming).length > 0 || Object.keys(fTiming).length > 0){
      renderTimingComparison(bTiming, fTiming, bExecData.error, fExecData.error);
    } else {
      document.getElementById('timing-container').innerHTML = '<div class="info">No timing data available.</div>';
    }

    // Show results from backward mode by default
    if(!bExecData.error && bExecData.columns && bExecData.columns.length > 0){
      let h = '<table class="results-table"><thead><tr>';
      bExecData.columns.forEach(c => h += '<th>'+escapeHtml(c)+'</th>');
      h += '</tr></thead><tbody>';
      bExecData.rows.forEach(row => {
        h += '<tr>';
        row.forEach(v => h += '<td>'+escapeHtml(v)+'</td>');
        h += '</tr>';
      });
      h += '</tbody></table>';
      document.getElementById('results-container').innerHTML = h;
      renderPlot(bExecData.columns, bExecData.rows);
    } else {
      renderPlot(null, null);
    }
    statusEl.textContent = 'Comparison ready — ' + stages.length + ' stage(s)';
  } catch(err){
    document.getElementById('compare-backward').textContent = 'Error: ' + err.message;
    document.getElementById('compare-forward').textContent = 'Error: ' + err.message;
    statusEl.textContent = 'Error';
  }
}

function showCompareStage(idx){
  const bMlir = (comparePipelineB && idx >= 0 && idx < comparePipelineB.length) ? comparePipelineB[idx].mlir : 'No data';
  const fMlir = (comparePipelineF && idx >= 0 && idx < comparePipelineF.length) ? comparePipelineF[idx].mlir : 'No data';
  document.getElementById('compare-backward').innerHTML = highlightMLIR(bMlir);
  document.getElementById('compare-forward').innerHTML  = highlightMLIR(fMlir);
}

// ── Timing Comparison (Forward vs Backward) ───────────────────────
function renderTimingComparison(bTiming, fTiming, bError, fError){
  const keys = [];
  PHASE_ORDER.forEach(k => { if(bTiming[k] !== undefined || fTiming[k] !== undefined) keys.push(k); });
  const allKeys = new Set([...Object.keys(bTiming), ...Object.keys(fTiming)]);
  allKeys.forEach(k => { if(!keys.includes(k)) keys.push(k); });

  const maxVal = Math.max(
    ...keys.map(k => Math.max(bTiming[k]||0, fTiming[k]||0)),
    0.001
  );

  let html = '<div class="timing-mode-legend">';
  html += '<span><span class="swatch" style="background:var(--blue)"></span> Backward (reverse)</span>';
  html += '<span><span class="swatch" style="background:var(--peach)"></span> Forward</span>';
  html += '</div>';

  if(bError) html += '<div style="color:var(--red);font-size:11px;margin-bottom:6px;">⚠ Backward: '+escapeHtml(bError)+'</div>';
  if(fError) html += '<div style="color:var(--red);font-size:11px;margin-bottom:6px;">⚠ Forward: '+escapeHtml(fError)+'</div>';

  keys.forEach(k => {
    const bMs = bTiming[k]||0;
    const fMs = fTiming[k]||0;
    const bPct = Math.max((bMs / maxVal) * 100, 0.5);
    const fPct = Math.max((fMs / maxVal) * 100, 0.5);
    html += '<div class="timing-bar-group">';
    html += '<div class="timing-label"><span>'+escapeHtml(k)+'</span></div>';
    html += '<div class="timing-compare-pair">';
    html += '<div class="timing-bar-track"><div class="timing-bar-fill" style="width:'+bPct+'%;background:var(--blue);"></div></div>';
    html += '<div class="timing-bar-track"><div class="timing-bar-fill" style="width:'+fPct+'%;background:var(--peach);"></div></div>';
    html += '</div></div>';
  });

  // Summary table
  const bTotal = keys.reduce((s,k) => s + (bTiming[k]||0), 0);
  const fTotal = keys.reduce((s,k) => s + (fTiming[k]||0), 0);

  html += '<div class="timing-summary"><table>';
  html += '<tr style="font-weight:600;"><th></th><th>Backward</th><th>Forward</th><th>Δ</th></tr>';
  keys.forEach(k => {
    const bMs = bTiming[k]||0;
    const fMs = fTiming[k]||0;
    const diff = fMs - bMs;
    const cls = Math.abs(diff) < 0.01 ? 'same' : (diff < 0 ? 'faster' : 'slower');
    const sign = diff >= 0 ? '+' : '';
    html += '<tr><td>'+escapeHtml(k)+'</td><td>'+bMs.toFixed(2)+' ms</td><td>'+fMs.toFixed(2)+' ms</td>';
    html += '<td class="timing-speedup '+cls+'">'+sign+diff.toFixed(2)+' ms</td></tr>';
  });
  const totalDiff = fTotal - bTotal;
  const totalCls = Math.abs(totalDiff) < 0.01 ? 'same' : (totalDiff < 0 ? 'faster' : 'slower');
  const totalSign = totalDiff >= 0 ? '+' : '';
  html += '<tr style="border-top:1px solid var(--overlay);font-weight:700;"><td>Total</td>';
  html += '<td>'+bTotal.toFixed(2)+' ms</td><td>'+fTotal.toFixed(2)+' ms</td>';
  html += '<td class="timing-speedup '+totalCls+'">'+totalSign+totalDiff.toFixed(2)+' ms</td></tr>';

  // Speedup ratio
  if(bTotal > 0 && fTotal > 0){
    const ratio = bTotal / fTotal;
    const faster = ratio > 1 ? 'Forward' : 'Backward';
    const speedup = ratio > 1 ? ratio : (1/ratio);
    html += '<tr><td colspan="4" style="text-align:center;padding-top:6px;font-size:12px;color:var(--text);">';
    html += '<strong>'+faster+'</strong> is <strong>'+speedup.toFixed(2)+'×</strong> faster overall';
    html += '</td></tr>';
  }
  html += '</table></div>';

  document.getElementById('timing-container').innerHTML = html;
}

function exitCompareMode(){
  inCompareMode = false;
  comparePipelineB = null;
  comparePipelineF = null;
  document.getElementById('code-view-single').style.display = '';
  document.getElementById('code-view-compare').style.display = 'none';
}

// ── Theme Toggle ───────────────────────────────────────────────────
function toggleTheme(){
  const html = document.documentElement;
  const btn = document.getElementById('theme-toggle');
  if(html.getAttribute('data-theme')==='light'){
    html.removeAttribute('data-theme');
    btn.innerHTML = '&#9788; Light';
  } else {
    html.setAttribute('data-theme','light');
    btn.innerHTML = '&#9790; Dark';
  }
}

// ── MLIR Syntax Highlighting ──────────────────────────────────────
function highlightMLIR(code){
  let lines = code.split('\n');
  // Track whether we are inside a derivative map block
  let inDerivMap = false;
  let braceDepth = 0;
  let derivMapDepth = 0;

  lines = lines.map(raw => {
    let s = escapeHtml(raw);

    // Track derivative map blocks: relalg.map that computes @d_* columns
    if (!inDerivMap && /relalg\.map.*computes\s*:/.test(raw) && /@\w+::@d_\w+/.test(raw)) {
      inDerivMap = true;
      derivMapDepth = (raw.match(/\{/g)||[]).length - (raw.match(/\}/g)||[]).length;
    } else if (inDerivMap) {
      derivMapDepth += (raw.match(/\{/g)||[]).length - (raw.match(/\}/g)||[]).length;
      if (derivMapDepth <= 0) inDerivMap = false;
    }

    // Classify line: derivative vs expression within a derivative map
    const isDerivLine = /@\w+::@d_\w+/.test(raw);
    const isExprLine = inDerivMap && !isDerivLine && /^\s+%\d+\s*=\s*(db\.|tuples\.getcol)/.test(raw);

    // Comments
    s = s.replace(/(\/\/.*)/g, '<span class="hl-comment">$1</span>');
    // String literals
    s = s.replace(/(&quot;[^&]*?&quot;)/g, '<span class="hl-string">$1</span>');

    // MLIR dialect keywords
    s = s.replace(/\b(module|func\.func|return|private|attributes)\b/g, '<span class="hl-keyword">$1</span>');

    // Specific arithmetic ops with semantic colors
    s = s.replace(/\b(db\.mul)\b/g, '<span class="hl-op-mul">$1</span>');
    s = s.replace(/\b(db\.add)\b/g, '<span class="hl-op-add">$1</span>');
    s = s.replace(/\b(db\.sub)\b/g, '<span class="hl-op-sub">$1</span>');
    s = s.replace(/\b(db\.div)\b/g, '<span class="hl-op-div">$1</span>');
    s = s.replace(/\b(db\.cmp)\b/g, '<span class="hl-op-cmp">$1</span>');

    // Runtime function calls — highlight the function name
    s = s.replace(/\b(db\.runtime_call)\s+(&quot;)(PowerFloat)(&quot;)/g,
      '<span class="hl-op">$1</span> $2<span class="hl-runtime-fn">$3</span>$4<span class="hl-math-badge hl-badge-pow">^</span>');
    s = s.replace(/\b(db\.runtime_call)\s+(&quot;)(Sig)(&quot;)/g,
      '<span class="hl-op">$1</span> $2<span class="hl-runtime-fn">$3</span>$4<span class="hl-math-badge hl-badge-sig">\u03c3</span>');
    s = s.replace(/\b(db\.runtime_call)\s+(&quot;)(Log)(&quot;)/g,
      '<span class="hl-op">$1</span> $2<span class="hl-runtime-fn">$3</span>$4<span class="hl-math-badge hl-badge-log">ln</span>');
    s = s.replace(/\b(db\.runtime_call)\s+(&quot;)(ArrayMMul)(&quot;)/g,
      '<span class="hl-op">$1</span> $2<span class="hl-runtime-fn">$3</span>$4<span class="hl-math-badge hl-badge-mmul">\u2297</span>');
    s = s.replace(/\b(db\.runtime_call)\s+(&quot;)(ArrayTranspose)(&quot;)/g,
      '<span class="hl-op">$1</span> $2<span class="hl-runtime-fn">$3</span>$4<span class="hl-math-badge hl-badge-transp">T</span>');
    s = s.replace(/\b(db\.runtime_call)\s+(&quot;)(ArrayScalar\w+)(&quot;)/g,
      '<span class="hl-op">$1</span> $2<span class="hl-runtime-fn">$3</span>$4');
    // Generic remaining runtime calls
    s = s.replace(/\b(db\.runtime_call)\s+(&quot;)(\w+)(&quot;)/g,
      '<span class="hl-op">$1</span> $2<span class="hl-runtime-fn">$3</span>$4');

    // Remaining dialect ops (not already colored)
    s = s.replace(/\b(relalg|subop|tuples|db|scf|util|dsa|memref|arith)\.(?!mul|add|sub|div|cmp|runtime_call)(\w+)/g, '<span class="hl-op">$1.$2</span>');

    // Types
    s = s.replace(/![\w.]+&lt;[^&]*&gt;/g, '<span class="hl-type">$&</span>');
    s = s.replace(/\b(i1|i8|i16|i32|i64|f32|f64|index)\b/g, '<span class="hl-type">$1</span>');
    s = s.replace(/!([\w.]+)/g, '<span class="hl-type">!$1</span>');

    // Attributes — derivative columns first (bold mauve)
    s = s.replace(/@(\w+)::@(d_\w+)/g, '<span class="hl-deriv-attr">@$1::@$2</span>');
    s = s.replace(/@(\w+)::@(\w+)/g, '<span class="hl-attr">@$1::@$2</span>');
    s = s.replace(/@(\w+)/g, '<span class="hl-attr">@$1</span>');

    // Numeric constants
    s = s.replace(/\b(\d+\.?\d*(?:e[+-]?\d+)?)\b/g, '<span class="hl-number">$1</span>');

    // SSA values (%name)
    s = s.replace(/%(\w+)/g, '<span style="color:var(--teal)">%$1</span>');

    // Add math badges for basic arithmetic ops (after line, subtle)
    if (/\bdb\.mul\b/.test(raw) && !/runtime_call/.test(raw))
      s += '<span class="hl-math-badge hl-badge-mul">\u00d7</span>';
    if (/\bdb\.add\b/.test(raw))
      s += '<span class="hl-math-badge hl-badge-add">+</span>';
    if (/\bdb\.sub\b/.test(raw))
      s += '<span class="hl-math-badge hl-badge-sub">\u2212</span>';

    // Wrap each line in a mlir-line span (with optional deriv/expr class)
    let cls = 'mlir-line';
    if (isDerivLine) cls += ' hl-deriv-line';
    else if (isExprLine) cls += ' hl-expr-line';
    // data-raw stores unescaped text for cursor matching
    return '<span class="' + cls + '" data-raw="' + escapeAttr(raw) + '">' + s + '</span>';
  });
  // display:block spans already break lines — no \n needed
  return lines.join('');
}
function escapeHtml(s){
  const d = document.createElement('div');
  d.textContent = String(s);
  return d.innerHTML;
}
function escapeAttr(s){
  return String(s).replace(/&/g,'&amp;').replace(/"/g,'&quot;').replace(/</g,'&lt;').replace(/>/g,'&gt;');
}

// ── Init ───────────────────────────────────────────────────────────
editorEl.value = tabs[0].content;
syncHighlight();
renderTabs();
fetch('/api/tables').then(r=>r.json()).then(data=>{
  if(data.tables && data.tables.length > 0){
    statusEl.textContent = 'Ready \u2014 '+data.tables.length+' table(s): '+data.tables.join(', ');
  }
}).catch(()=>{});
</script>
</body>
</html>
)HTML";

// ---------------------------------------------------------------------------
// Custom ResultProcessor that captures the arrow::Table
// ---------------------------------------------------------------------------
class TableCapture : public execution::ResultProcessor {
   std::shared_ptr<arrow::Table>& result;
public:
   TableCapture(std::shared_ptr<arrow::Table>& result) : result(result) {}
   void process(runtime::ExecutionContext* executionContext) override {
      auto resultTable = executionContext->getResultOfType<runtime::ResultTable>(0);
      if (!resultTable) return;
      result = resultTable.value()->get();
   }
};

// ---------------------------------------------------------------------------
// Custom TimingProcessor that captures per-phase timing
// ---------------------------------------------------------------------------
class TimingCapture : public execution::TimingProcessor {
   std::unordered_map<std::string, double> timing;
public:
   void addTiming(const std::unordered_map<std::string, double>& t) override {
      timing.insert(t.begin(), t.end());
   }
   void process() override {} // intentionally empty — we read timing later
   std::unordered_map<std::string, double>& getTimeMap() override { return timing; }
};

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char** argv) {
   int port = 8080;
   std::string dbDir;

   if (argc < 2) {
      std::cerr << "USAGE: sql-web <database> [port]" << std::endl;
      std::cerr << "  database: path to database directory" << std::endl;
      std::cerr << "  port: HTTP port (default: 8080)" << std::endl;
      return 1;
   }
   dbDir = std::string(argv[1]);
   if (argc >= 3) {
      port = std::atoi(argv[2]);
   }

   auto session = runtime::Session::createSession(dbDir, true);
   support::eval::init();

   // ── Load sample datasets ────────────────────────────────────────────
   {
      std::cout << "Loading sample datasets..." << std::flush;
      // Execute a single SQL statement
      auto runStmt = [&](const std::string& sql) {
         try {
            auto cfg = execution::createQueryExecutionConfig(execution::ExecutionMode::DEFAULT, true);
            cfg->exitOnError = false;
            auto executer = execution::QueryExecuter::createDefaultExecuter(std::move(cfg), *session);
            executer->fromData(sql);
            executer->execute();
         } catch (const std::exception& e) {
            std::cerr << "\n  Warning: " << e.what() << std::endl;
         } catch (...) {}
      };

      // Gradient descent sample data
      runStmt("create table if not exists data (x float, y float)");
      runStmt("insert into data (select *, 5*x+10 from (select random() x from generate_series(1,10)) tmp)");

      // Iris dataset (array-based for neural network training)
      runStmt("create table if not exists iris (sepal_length float, sepal_width float, petal_length float, petal_width float, species int)");
      runStmt("copy iris from './iris.csv' delimiter ',' HEADER");
      runStmt("create table if not exists iris3 (img float[], one_hot float[])");
      runStmt("insert into iris3 (select array[[sepal_length/10,sepal_width/10,petal_length/10,petal_width/10]] as img, "
              "array[(array_fill(0::float,array[species]) || 1::float ) || array_fill(0::float,array[2-species])] as one_hot from iris limit 150)");

      std::cout << " done." << std::endl;
   }

   httplib::Server svr;

   // GET / — Serve HTML page
   svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
      res.set_content(HTML_PAGE, "text/html");
   });

   // POST /api/execute — Execute SQL, return {columns, rows, timing}
   svr.Post("/api/execute", [&session](const httplib::Request& req, httplib::Response& res) {
      std::lock_guard<std::mutex> lock(executionMutex);
      try {
         auto json = nlohmann::json::parse(req.body);
         std::string sql = json["sql"];

         std::shared_ptr<arrow::Table> resultTable;
         auto queryExecutionConfig = execution::createQueryExecutionConfig(execution::ExecutionMode::DEFAULT, true);
         queryExecutionConfig->exitOnError = false;
         queryExecutionConfig->resultProcessor = std::make_unique<TableCapture>(resultTable);
         auto* timingPtr = new TimingCapture();
         queryExecutionConfig->timingProcessor = std::unique_ptr<execution::TimingProcessor>(timingPtr);
         auto executer = execution::QueryExecuter::createDefaultExecuter(std::move(queryExecutionConfig), *session);
         executer->fromData(sql);
         executer->execute();

         nlohmann::json response;
         if (resultTable) {
            response = arrowTableToJson(resultTable);
         } else {
            response["columns"] = nlohmann::json::array();
            response["rows"] = nlohmann::json::array();
         }

         nlohmann::json timingJson;
         for (auto& [name, val] : timingPtr->getTimeMap()) {
            timingJson[name] = val;
         }
         response["timing"] = timingJson;

         res.set_content(response.dump(), "application/json");
      } catch (const std::exception& e) {
         nlohmann::json err;
         err["error"] = e.what();
         res.set_content(err.dump(), "application/json");
      } catch (...) {
         nlohmann::json err;
         err["error"] = "Unknown error during execution";
         res.set_content(err.dump(), "application/json");
      }
   });

   // POST /api/mlir — Translate SQL to canonical MLIR
   svr.Post("/api/mlir", [&session](const httplib::Request& req, httplib::Response& res) {
      std::lock_guard<std::mutex> lock(executionMutex);
      try {
         auto json = nlohmann::json::parse(req.body);
         std::string sql = json["sql"];
         auto catalog = session->getCatalog();
         std::string mlir = translateToMLIR(sql, *catalog);
         nlohmann::json response;
         response["mlir"] = mlir;
         res.set_content(response.dump(), "application/json");
      } catch (const std::exception& e) {
         nlohmann::json err;
         err["error"] = std::string("MLIR generation failed: ") + e.what();
         res.set_content(err.dump(), "application/json");
      }
   });

   // POST /api/pipeline — Generate IR at all pipeline stages
   svr.Post("/api/pipeline", [&session](const httplib::Request& req, httplib::Response& res) {
      std::lock_guard<std::mutex> lock(executionMutex);
      try {
         auto json = nlohmann::json::parse(req.body);
         std::string sql = json["sql"];
         nlohmann::json stages = generatePipelineStages(sql, *session);
         nlohmann::json response;
         response["stages"] = stages;
         res.set_content(response.dump(), "application/json");
      } catch (const std::exception& e) {
         nlohmann::json err;
         err["error"] = std::string("Pipeline generation failed: ") + e.what();
         res.set_content(err.dump(), "application/json");
      }
   });

   // GET /api/tables — List database tables
   svr.Get("/api/tables", [&session](const httplib::Request&, httplib::Response& res) {
      std::lock_guard<std::mutex> lock(executionMutex);
      try {
         auto catalog = session->getCatalog();
         auto names = catalog->getTableNames();
         std::sort(names.begin(), names.end());
         nlohmann::json response;
         response["tables"] = names;
         res.set_content(response.dump(), "application/json");
      } catch (const std::exception& e) {
         nlohmann::json err;
         err["error"] = e.what();
         res.set_content(err.dump(), "application/json");
      }
   });

   std::cout << "∂SQL web interface starting on http://localhost:" << port << std::endl;
   std::cout << "Database: " << dbDir << std::endl;
   std::cout << "Press Ctrl-C to stop." << std::endl;

   if (!svr.listen("0.0.0.0", port)) {
      std::cerr << "Failed to start server on port " << port << std::endl;
      return 1;
   }

   return 0;
}
