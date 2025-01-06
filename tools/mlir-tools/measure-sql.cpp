#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>

#include "execution/Execution.h"
#include "execution/Timing.h"
#include "mlir-support/eval.h"

#include <stdlib.h>

void writeToCSV(std::unordered_map<std::string, double>& data, std::string filePath);
std::unordered_map<std::string, std::vector<double>> readCSV(std::string filePath);
double calcMean(std::vector<double> data);
double calcStdDev(std::vector<double> data);

int main(int argc, char** argv) {
    if (argc <= 4) {
       std::cerr << "USAGE: run-sql *.sql database number_of_repetitions *.csv" << std::endl;
       return 1;
    }
    std::string inputFileName = std::string(argv[1]);
    std::string directory = std::string(argv[2]);
    size_t repetitions = std::atoi(argv[3]);
    std::filesystem::path timingFile = std::string(argv[4]);

    // file paths
    std::filesystem::path folder = timingFile.parent_path();
    std::string fileNameNoExt = timingFile.stem().string();
    std::string filePathNoExt = (folder / fileNameNoExt).string();
    std::string timingFileExtension = timingFile.extension().string();
    std::string timingRawDataFile = filePathNoExt + "_raw" + timingFileExtension;
    std::string timingProcessedDataFile = filePathNoExt + "_processed.csv" + timingFileExtension;

    // loading database
    std::cout << "Loading Database from: " << directory << '\n';
    auto session = runtime::Session::createSession(directory,false);        
    support::eval::init();

    // rediecting stdout to /dev/null/ so that it doesn't get all printed to the terminal
    std::ofstream file("/dev/null");
    std::streambuf *buffer = std::cout.rdbuf();
    std::cout.rdbuf(file.rdbuf());

    // running the measurements
    for(size_t i = 0; i < repetitions; i++) {
        if(i % 100 == 0) {
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
        writeToCSV(timing, timingRawDataFile);
    }
    std::cout.rdbuf(buffer);

    // process data, i.e. calculate mean and standard deviation.
    auto csv = readCSV(timingRawDataFile);
    std::unordered_map<std::string, double> processedTiming;
    for (auto [name, t] : csv) {
        std::string nameMean = name + "_mean";
        std::string namestdDev = name + "_stdDev";
        processedTiming[nameMean] = calcMean(t);
        processedTiming[namestdDev] = calcStdDev(t);
    }

    // Store the values in new file
    writeToCSV(processedTiming, timingProcessedDataFile);
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