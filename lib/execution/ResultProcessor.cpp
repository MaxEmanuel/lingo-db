#include <iomanip>
#include <iostream>

#define NDEBUG
#include <arrow/array.h>
#include <arrow/datum.h> // Include the header file for arrow::Datum
#include <arrow/pretty_print.h>
#include <arrow/scalar.h>
#include <arrow/table.h>
#include <arrow/visit_array_inline.h>
#undef NDEBUG

#include "execution/ResultProcessing.h"
#include "runtime/TableBuilder.h"
#include <functional>
#include <arrow/status.h>

namespace {
unsigned char hexval(unsigned char c) {
   if ('0' <= c && c <= '9')
      return c - '0';
   else if ('a' <= c && c <= 'f')
      return c - 'a' + 10;
   else if ('A' <= c && c <= 'F')
      return c - 'A' + 10;
   else
      abort();
}

class TableRetriever : public execution::ResultProcessor {
   std::shared_ptr<arrow::Table>& result;

   public:
   TableRetriever(std::shared_ptr<arrow::Table>& result) : result(result) {}
   void process(runtime::ExecutionContext* executionContext) override {
      auto resultTable = executionContext->getResultOfType<runtime::ResultTable>(0);
      if (!resultTable) return;
      result = resultTable.value()->get();
   }
};

class PrintHalfFloat : public arrow::ArrayVisitor {
   std::ostringstream partial;

   public:
   arrow::Result<std::string> Compute(std::shared_ptr<arrow::Array> array) {
      ARROW_RETURN_NOT_OK(arrow::VisitArrayInline(*array, this));
      return partial.str();
   }

   // Default implementation
   arrow::Status Visit(const arrow::Array& array) {
      return arrow::Status::NotImplemented("Can not compute sum for array of type ", array.type()->ToString());
   }

   arrow::Status Visit(const arrow::HalfFloatArray& array) {
      unsigned index = 0;
      for (std::optional<typename arrow::HalfFloatType::c_type> value : array) {
         if (value.has_value()) {
            unsigned int proc = static_cast<unsigned>(value.value()) << 16;

            float tmp = *reinterpret_cast<float*>(&proc);
            partial << tmp;
            if (index < array.length() - 1) {
               partial << ",\n";
            }
            ++index;
         }
      }

      return arrow::Status::OK();
   }
};

void printTable(const std::shared_ptr<arrow::Table>& table) {
   // Do not output anything for insert or copy statements
   if (table->columns().empty()) {
      std::cout << "Statement executed successfully." << std::endl;
      return;
   }

   std::vector<std::string> columnReps;
   std::vector<size_t> positions;
   arrow::PrettyPrintOptions options;
   options.indent_size = 0;
   options.window = 100;
   std::cout << "|";
   std::string rowSep = "-";
   std::vector<bool> convertHex;
   for (auto c : table->columns()) {
      std::cout << std::setw(30) << table->schema()->field(positions.size())->name() << "  |";
      convertHex.push_back(table->schema()->field(positions.size())->type()->id() == arrow::Type::FIXED_SIZE_BINARY);
      rowSep += std::string(33, '-');
      std::stringstream sstr;
      if (table->schema()->field(positions.size())->type()->id() != arrow::Type::HALF_FLOAT) {
         arrow::PrettyPrint(*c.get(), options, &sstr); //NOLINT (clang-diagnostic-unused-result)
      } else {
         sstr << "[\n[\n";
         PrintHalfFloat printer;
         for (unsigned i = 0; i < c->num_chunks(); i++) {
            sstr << printer.Compute(c->chunk(i)).ValueOrDie();
            if (i < c->num_chunks() - 1) {
               sstr << ",\n";
            }
         }
         sstr << "\n]\n]";
      }

      columnReps.push_back(sstr.str());
      positions.push_back(0);
   }
   std::cout << std::endl
             << rowSep << std::endl;
   bool cont = true;
   while (cont) {
      cont = false;
      bool skipNL = false;
      for (size_t column = 0; column < columnReps.size(); column++) {
         char lastHex = 0;
         bool first = true;
         std::stringstream out;
         while (positions[column] < columnReps[column].size()) {
            cont = true;
            char curr = columnReps[column][positions[column]];
            char next = columnReps[column][positions[column] + 1];
            positions[column]++;
            if (first && (curr == '[' || curr == ']' || curr == ',')) {
               continue;
            }
            if (curr == ',' && next == '\n') {
               continue;
            }
            if (curr == '\n') {
               break;
            } else {
               if (convertHex[column]) {
                  if (isxdigit(curr)) {
                     if (lastHex == 0) {
                        first = false;
                        lastHex = curr;
                     } else {
                        char converted = (hexval(lastHex) << 4 | hexval(curr));
                        out << converted;
                        lastHex = 0;
                     }
                  } else {
                     first = false;
                     out << curr;
                  }
               } else {
                  first = false;
                  out << curr;
               }
            }
         }
         if (first) {
            skipNL = true;
         } else {
            if (column == 0) {
               std::cout << "|";
            }
            std::cout << std::setw(30) << out.str() << "  |";
         }
      }
      if (!skipNL) {
         std::cout << "\n";
      }
   }
}

class TablePrinter : public execution::ResultProcessor {
   void process(runtime::ExecutionContext* executionContext) override {
      auto resultTable = executionContext->getResultOfType<runtime::ResultTable>(0);
      if (!resultTable) return;
      auto table = resultTable.value()->get();
      printTable(table);
   }
};
class BatchedTablePrinter : public execution::ResultProcessor {
   void process(runtime::ExecutionContext* executionContext) override {
      for (size_t i = 0;; i++) {
         auto resultTable = executionContext->getResultOfType<runtime::ResultTable>(i);
         if (!resultTable) return;
         auto table = resultTable.value()->get();
         printTable(table);
      }
   }
};
} // namespace

std::unique_ptr<execution::ResultProcessor> execution::createTableRetriever(std::shared_ptr<arrow::Table>& result) {
   return std::make_unique<TableRetriever>(result);
}

std::unique_ptr<execution::ResultProcessor> execution::createTablePrinter() {
   return std::make_unique<TablePrinter>();
}

std::unique_ptr<execution::ResultProcessor> execution::createBatchedTablePrinter() {
   return std::make_unique<BatchedTablePrinter>();
}