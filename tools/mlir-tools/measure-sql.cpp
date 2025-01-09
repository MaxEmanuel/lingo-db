#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>

#include "execution/Execution.h"
#include "execution/Timing.h"
#include "mlir-support/eval.h"

#include <stdlib.h>

void writeToCSV(std::unordered_map<std::string, double>& data, std::string filePath);
std::unordered_map<std::string, std::vector<double>> readCSV(std::string filePath);
double calcMean(std::vector<double> data);
double calcStdDev(std::vector<double> data);
std::vector<double> calcCompilationTime(std::vector<std::unordered_map<std::string, double>> dataVec);

int main(int argc, char** argv) {
    if (argc <= 4) {
       std::cerr << "USAGE: run-sql *.sql database number_of_repetitions [base file name]" << std::endl;
       return 1;
    }
    // arguments
    std::string inputFileName = std::string(argv[1]);
    std::string directory = std::string(argv[2]);
    size_t repetitions = std::atoi(argv[3]);
    std::string baseFileName = std::string(argv[4]);

    // file paths
    std::string timeRawFile = baseFileName + "_raw.csv";
    std::string timeProcessedFile = baseFileName + "_processed.csv";
    std::string compTimeRawFile = baseFileName + "Summarized_raw.csv";
    std::string compTimeProcessedFile = baseFileName + "Summarized_processed.csv";

    // loading database
    std::cout << "Loading Database from: " << directory << '\n';
    auto session = runtime::Session::createSession(directory,false);        
    support::eval::init();

    // rediecting stdout to /dev/null/ so that it doesn't get all printed to the terminal
    std::ofstream file("/dev/null");
    std::streambuf *buffer = std::cout.rdbuf();
    std::cout.rdbuf(file.rdbuf());
    std::vector<std::unordered_map<std::string, double>> times;

    // running the measurements
    for(size_t i = 0; i < repetitions; i++) {
        if(i % 100 == 0 || i == (repetitions - 1)) {
            std::cout.rdbuf(buffer);
            std::cout << "Iteration: " << i << std::endl;
            std::cout.rdbuf(file.rdbuf());
        }
        execution::ExecutionMode runMode = execution::getExecutionMode();
        auto queryExecutionConfig = execution::createQueryExecutionConfig(runMode, true);

        unsetenv("PERF_BUILDID_DIR");
        queryExecutionConfig->timingProcessor = std::make_unique<execution::TimingPrinter>(inputFileName);
        execution::QueryExecutionConfig* rawPtr = queryExecutionConfig.get();
        auto executer = execution::QueryExecuter::createDefaultExecuter(std::move(queryExecutionConfig), *session);
        executer->fromFile(inputFileName);
        executer->execute();

        // write time data to raw data file
        auto timing = rawPtr->timingProcessor->getTimeMap();
        times.push_back(timing);
        writeToCSV(timing, timeRawFile);
    }
    std::cout.rdbuf(buffer);

    // process data, i.e. calculate mean and standard deviation.
    auto csv = readCSV(timeRawFile);
    std::unordered_map<std::string, double> processedTiming;
    for (auto [name, t] : csv) {
        std::string nameMean = name + "_mean";
        std::string namestdDev = name + "_stdDev";
        processedTiming[nameMean] = calcMean(t);
        processedTiming[namestdDev] = calcStdDev(t);
    }

    // Store the values in new file
    writeToCSV(processedTiming, timeProcessedFile);

    std::unordered_map<std::string, std::vector<double>> comp;
    comp["execution"] = csv["executionTime"];
    comp["optimization"] = csv["QOpt"];
    comp["compilation"] = calcCompilationTime(times);

    std::vector<std::unordered_map<std::string, double>> result(comp.begin()->second.size());
    for (const auto& [key, vec] : comp) {
        for (size_t i = 0; i < vec.size(); ++i) {
            result[i][key] = vec[i];
        }
    }

    for(auto row : result) {
        writeToCSV(row, compTimeRawFile);
    }

    std::unordered_map<std::string, double> processedCompTiming;
    for (auto [name, t] : comp) {
        std::string nameMean = name + "_mean";
        std::string namestdDev = name + "_stdDev";
        processedCompTiming[nameMean] = calcMean(t);
        processedCompTiming[namestdDev] = calcStdDev(t);
    }

    // Store the values in new file
    writeToCSV(processedCompTiming, compTimeProcessedFile);
    
    return 0;
}

void writeToCSV(std::unordered_map<std::string, double>& data, std::string filePath) {
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::app);
    if (!file.is_open()) {
        throw std::runtime_error("Error opening file!");
    }
    // If the file is empty
    file.seekg(0, std::ios::end);
    size_t num = data.size();
    if (file.tellg() == 0) {
        // File is empty, write the header
        size_t i = 0;
        for (auto entry : data) {
            file << entry.first;
            if (i != (num - 1)) {
                file << ", ";
            }
            i++;
        }
        file << "\n";
    }

    // Write the data rows
    size_t j = 0;
    for (auto entry : data) {
        file << entry.second;
        if (j != (num - 1)) {
            file << ", ";
        }
        j++;
    }
    file << "\n";
    file.close();
}

std::unordered_map<std::string, std::vector<double>> readCSV(std::string filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Error opening file!");
    }
    std::unordered_map<std::string, std::vector<double>> csvData;
    std::string line;

    // get the first line, defining the header
    std::getline(file, line);
    std::stringstream headerStream(line);
    std::string headerCell;
    std::vector<std::string> headerList;

    // create a emtpy vector for each header
    while (std::getline(headerStream, headerCell, ',')) {
        headerCell = headerCell.substr(headerCell.find_first_not_of(' '), headerCell.find_last_not_of(' ') - headerCell.find_first_not_of(' ') + 1);
        csvData[headerCell] = std::vector<double>();
        headerList.push_back(headerCell);
    }
    // Read the file line by line, starting at the second line
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        
        size_t i = 0;
        while (std::getline(ss, cell, ',')) {
            csvData[headerList[i]].push_back(std::stod(cell));
            i++;
        }
    }

    // Close the file
    file.close();
    return csvData;
}

double calcMean(std::vector<double> data) {
    double sum = 0.0;
    for(const auto& num : data) {
        sum += num;
    }

    double mean = sum / data.size();
    return mean;
}

double calcStdDev(std::vector<double> data) {
    double mean = calcMean(data);
    double deviation = 0.0;
    for(const auto& num : data) {
        double d = std::pow((num - mean), 2);
        deviation += d;
    }
    double stdDev = std::sqrt(deviation / (data.size() - 1));
    return stdDev;
}

std::vector<double> calcCompilationTime(std::vector<std::unordered_map<std::string, double>> dataVec) {
    std::vector<std::string> compParts = {"lowerRelAlg", "lowerSubOp", "lowerDB", "lowerDSA", "lowerToLLVM", "toLLVMIR", "llvmOptimize", "llvmCodeGen"};
    std::vector<double> compTime(dataVec.size(), 0.0);
    for(size_t i = 0; i < dataVec.size(); i++) {
        for(auto [name, time] : dataVec[i]) {
            if(std::find(compParts.begin(), compParts.end(), name) != compParts.end()) {
                compTime[i] += time;
            }
        }
    }
    return compTime;
}