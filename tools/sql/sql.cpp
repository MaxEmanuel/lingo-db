#include <csignal>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <readline/readline.h>
#include <readline/history.h>

#include "execution/Execution.h"
#include "execution/Frontend.h"
#include "frontend/SQL/Parser.h"
#include "mlir-support/eval.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/SubOperator/SubOperatorDialect.h"
#include "mlir/Dialect/SubOperator/SubOperatorOps.h"
#include "mlir/IR/BuiltinOps.h"
#include "runtime/Session.h"

static volatile sig_atomic_t interrupted = 0;

static void sigintHandler(int) {
   interrupted = 1;
}

void handleQuery(runtime::Session& session, const std::string& sqlQuery) {
   try {
      auto queryExecutionConfig = execution::createQueryExecutionConfig(execution::ExecutionMode::DEFAULT, true);
      queryExecutionConfig->exitOnError = false;
      auto executer = execution::QueryExecuter::createDefaultExecuter(std::move(queryExecutionConfig), session);
      executer->fromData(sqlQuery);
      executer->execute();
   } catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
   }
}

std::string getMLIR(const std::string& sql, runtime::Catalog& catalog) {
   mlir::MLIRContext context;
   execution::initializeContext(context);
   mlir::OpBuilder builder(&context);
   mlir::ModuleOp moduleOp = builder.create<mlir::ModuleOp>(builder.getUnknownLoc());
   try {
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
      mlir::func::FuncOp funcOp = builder.create<mlir::func::FuncOp>(builder.getUnknownLoc(), "main", builder.getFunctionType({}, {}));
      funcOp.getBody().push_back(queryBlock);
      std::string result;
      llvm::raw_string_ostream os(result);
      mlir::OpPrintingFlags flags;
      flags.assumeVerified();
      moduleOp->print(os, flags);
      return result;
   } catch (std::exception& e) {
      return std::string("Error: ") + e.what();
   }
}

void printHelp() {
   std::cout << "Backslash commands:\n"
             << "  \\q        Quit\n"
             << "  \\?        Show this help\n"
             << "  \\i FILE   Execute SQL from file\n"
             << "  \\d        List tables\n"
             << "  \\d TABLE  Describe table\n"
             << "  \\timing   Toggle timing display\n"
             << "  \\mlir QUERY  Show MLIR for query\n";
}

void executeFile(runtime::Session& session, const std::string& filename) {
   std::ifstream infile(filename);
   if (!infile.is_open()) {
      std::cerr << "Could not open file: " << filename << std::endl;
      return;
   }
   std::stringstream buffer;
   buffer << infile.rdbuf();
   std::string content = buffer.str();

   // Split on semicolons and execute each statement
   std::stringstream ss(content);
   std::string line;
   std::stringstream query;
   while (std::getline(ss, line)) {
      // Handle \i directive for file inclusion
      auto trimPos = line.find_first_not_of(" \t");
      if (trimPos != std::string::npos && line.substr(trimPos, 2) == "\\i") {
         // Execute any pending query first
         std::string q = query.str();
         if (!q.empty() && q.find_first_not_of(" \t\n\r") != std::string::npos) {
            handleQuery(session, q);
         }
         query.str("");
         query.clear();
         // Extract filename and execute it
         std::string incFile = line.substr(trimPos + 2);
         auto fpos = incFile.find_first_not_of(" \t");
         if (fpos != std::string::npos) {
            incFile = incFile.substr(fpos);
            // Resolve relative to the directory of the current file
            std::filesystem::path incPath(incFile);
            if (incPath.is_relative()) {
               incPath = std::filesystem::path(filename).parent_path() / incPath;
            }
            executeFile(session, incPath.string());
         }
         continue;
      }
      query << line << "\n";
      // Check if line ends with ';' but not inside a comment
      auto commentPos = line.find("--");
      auto semiPos = line.rfind(';');
      if (semiPos != std::string::npos && (commentPos == std::string::npos || semiPos < commentPos)) {
         std::string q = query.str();
         if (!q.empty()) {
            handleQuery(session, q);
         }
         query.str("");
         query.clear();
      }
   }
   // Execute any remaining query
   std::string remaining = query.str();
   if (!remaining.empty() && remaining.find_first_not_of(" \t\n\r") != std::string::npos) {
      handleQuery(session, remaining);
   }
}

void listTables(runtime::Session& session) {
   auto names = session.getCatalog()->getTableNames();
   if (names.empty()) {
      std::cout << "No tables." << std::endl;
      return;
   }
   std::cout << "Tables:" << std::endl;
   for (const auto& name : names) {
      std::cout << "  " << name << std::endl;
   }
}

void describeTable(runtime::Session& session, const std::string& tableName) {
   auto relation = session.getCatalog()->findRelation(tableName);
   if (!relation) {
      std::cerr << "Table not found: " << tableName << std::endl;
      return;
   }
   auto metaData = relation->getMetaData();
   if (!metaData) {
      std::cerr << "No metadata for table: " << tableName << std::endl;
      return;
   }
   auto& columns = metaData->getOrderedColumns();
   std::cout << "Table: " << tableName << std::endl;
   for (const auto& col : columns) {
      auto colMD = metaData->getColumnMetaData(col);
      auto& colType = colMD->getColumnType();
      std::cout << "  " << col << " : " << colType.base
                << (colType.nullable ? " (nullable)" : "") << std::endl;
   }
}

bool handleBackslash(runtime::Session& session, const std::string& cmd, bool& showTiming) {
   if (cmd == "\\q") {
      return true; // signal quit
   } else if (cmd == "\\?") {
      printHelp();
   } else if (cmd.substr(0, 2) == "\\i") {
      std::string filename = cmd.substr(2);
      // trim leading whitespace
      auto pos = filename.find_first_not_of(" \t");
      if (pos != std::string::npos) {
         filename = filename.substr(pos);
      }
      if (filename.empty()) {
         std::cerr << "Usage: \\i FILE" << std::endl;
      } else {
         executeFile(session, filename);
      }
   } else if (cmd == "\\d") {
      listTables(session);
   } else if (cmd.substr(0, 2) == "\\d" && cmd.size() > 2) {
      std::string tableName = cmd.substr(2);
      auto pos = tableName.find_first_not_of(" \t");
      if (pos != std::string::npos) {
         tableName = tableName.substr(pos);
      }
      // trim trailing whitespace
      auto endPos = tableName.find_last_not_of(" \t");
      if (endPos != std::string::npos) {
         tableName = tableName.substr(0, endPos + 1);
      }
      describeTable(session, tableName);
   } else if (cmd == "\\timing") {
      showTiming = !showTiming;
      std::cout << "Timing is " << (showTiming ? "on" : "off") << "." << std::endl;
   } else if (cmd.substr(0, 5) == "\\mlir") {
      std::string query = cmd.substr(5);
      auto pos = query.find_first_not_of(" \t");
      if (pos != std::string::npos) {
         query = query.substr(pos);
      }
      if (query.empty()) {
         std::cerr << "Usage: \\mlir QUERY" << std::endl;
      } else {
         std::cout << getMLIR(query, *session.getCatalog()) << std::endl;
      }
   } else {
      std::cerr << "Unknown command: " << cmd << std::endl;
      std::cerr << "Type \\? for help." << std::endl;
   }
   return false;
}

int main(int argc, char** argv) {
   if (argc <= 1) {
      std::cerr << "USAGE: sql database" << std::endl;
      return 1;
   }
   auto session = runtime::Session::createSession(std::string(argv[1]), true);
   support::eval::init();

   // Set up signal handler
   struct sigaction sa;
   sa.sa_handler = sigintHandler;
   sigemptyset(&sa.sa_mask);
   sa.sa_flags = 0;
   sigaction(SIGINT, &sa, nullptr);

   bool showTiming = false;

   // Check if we have additional file arguments to execute
   if (argc > 2) {
      for (int i = 2; i < argc; i++) {
         executeFile(*session, std::string(argv[i]));
      }
      return 0;
   }

   // Interactive REPL
   using_history();

   std::string historyFile;
   if (const char* home = std::getenv("HOME")) {
      historyFile = std::string(home) + "/.lingodb_history";
      read_history(historyFile.c_str());
   }

   while (true) {
      interrupted = 0;
      char* line = readline("sql> ");
      if (!line) {
         // EOF
         std::cout << std::endl;
         break;
      }
      std::string input(line);
      free(line);

      // Skip empty lines
      if (input.empty()) continue;

      // Handle backslash commands
      if (input[0] == '\\') {
         add_history(input.c_str());
         if (handleBackslash(*session, input, showTiming)) {
            break;
         }
         continue;
      }

      // Read multi-line query until semicolon (not inside a -- comment)
      std::stringstream query;
      query << input;
      while (true) {
         // Check if current line ends with ';' outside a comment
         auto commentPos = input.find("--");
         auto semiPos = input.rfind(';');
         if (semiPos != std::string::npos && (commentPos == std::string::npos || semiPos < commentPos)) {
            break;
         }
         char* cont = readline("  -> ");
         if (!cont) break;
         input = std::string(cont);
         free(cont);
         query << "\n" << input;
      }

      std::string fullQuery = query.str();
      if (!fullQuery.empty()) {
         add_history(fullQuery.c_str());

         auto start = std::chrono::high_resolution_clock::now();
         handleQuery(*session, fullQuery);
         auto end = std::chrono::high_resolution_clock::now();

         if (showTiming) {
            auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0;
            std::cout << "Time: " << ms << " ms" << std::endl;
         }
      }
   }

   if (!historyFile.empty()) {
      write_history(historyFile.c_str());
   }

   return 0;
}
