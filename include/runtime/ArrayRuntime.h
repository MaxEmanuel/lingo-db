#ifndef RUNTIME_ARRAYRUNTIME_H
#define RUNTIME_ARRAYRUNTIME_H
#include "runtime/helpers.h"
#include <vector>
#include <memory>
#include <type_traits>
#include <string>
#include <sstream>
#include <cstring>
#include <algorithm>

namespace runtime
{

    template<typename T>
    class Array {
        std::vector<std::unique_ptr<T>> vector;
        std::vector<std::unique_ptr<std::vector<std::unique_ptr<T>>>> matrix;
        int dimensions;

        public:
        Array(runtime::VarLen32 array, int dimensions) : dimensions(dimensions) {
            std::string content = array.str();
            if (dimensions == 1) {
                this->vector = std::vector<std::unique_ptr<T>>();
                this->toVector(content, this->vector);
            } else if (dimensions == 2) {
                this->matrix = std::vector<std::unique_ptr<std::vector<std::unique_ptr<T>>>>();
                this->toMatrix(content);
            } else {
                throw std::runtime_error("More than 2 dimensions are currently not supported");
            }
        };

        runtime::VarLen32 toString() {
            std::string result;

            if (this->dimensions == 2) {
                result = this->matrixToString();
            } else {
                result = this->vectorToString(this->vector);
            }

            char* data = new char[result.length()];           
            memcpy(data, result.data(), result.length());     
            return runtime::VarLen32((uint8_t*) data, result.length());
        };

        private:
        void toVector(std::string array, std::vector<std::unique_ptr<T>>& container) {
            // Delete spaces if content is not std::string
            array.erase(std::remove_if(array.begin(), array.end(), ::isspace), array.end());
            // Delete outer brackets ({})
            array = array.substr(1, array.size() - 2);
            std::stringstream stringStream(array);
            std::string singleValue;
            while(std::getline(stringStream, singleValue, ',')) {
                if (singleValue.find("Null") != std::string::npos || singleValue.find("NULL") != std::string::npos|| singleValue.find("null") != std::string::npos){
                    container.push_back(nullptr);
                } else {
                    T value = this->castToNumber(singleValue);
                    container.push_back(std::make_unique<T>(value));
                }
            }
        };

        T castToNumber(std::string element) {
            if (std::is_same<T, int32_t>::value) {
                return std::stoi(element);
            } else if (std::is_same<T, int64_t>::value) {
                return std::stoll(element);
            } else if (std::is_same<T, float>::value){
                return std::stof(element);
            } else if (std::is_same<T, double>::value) {
                return std::stod(element);
            } else {
                throw std::runtime_error("The element - " + element + " - cannot be converted");
            }
        };

        void toMatrix(std::string array) {
            // Remove all spaces
            array.erase(std::remove_if(array.begin(), array.end(), ::isspace), array.end());
            // Remove outer brackets ({})
            array = array.substr(1, array.size() - 2);
            std::stringstream stringStream(array);
            std::string singleVector;
            // Iterate over each vector element of the matrix
            while (std::getline(stringStream, singleVector, '}')) {
                if (singleVector == "NULL" || singleVector == "Null" || singleVector == "null"){
                    this->matrix.push_back(nullptr);
                } else {
                    std::unique_ptr<std::vector<std::unique_ptr<T>>> vector = std::make_unique<std::vector<std::unique_ptr<T>>>();
                    // Delete "," if it is in front
                    if (singleVector[0] == ',') {
                        singleVector = singleVector.substr(1, singleVector.size() - 1);
                    }
                    if (singleVector[0] != '{') {
                        throw std::runtime_error("The elements of the array does not correspond with the specification of the dimensions number");
                    }
                    // Add the deleted "}" from the "getline" function
                    singleVector = singleVector + '}';
                    this->toVector(singleVector, *vector);
                    this->matrix.push_back(std::move(vector)); 
                }                
            }
        };

        std::string vectorToString(std::vector<std::unique_ptr<T>>& container) {
            std::string result = "{";
            for (auto& element : container) {
                if (element == nullptr) {
                    result += "null, ";
                } else {
                    result += std::to_string(*element) + ", ";
                }
            }
            result = result.substr(0, result.size() - 2);
            result += "}";
            return result;
        };

        std::string matrixToString() {
            std::string result = "{";
            for (auto& element : this->matrix) {
                if (element == nullptr) {
                    result += "null, ";
                } else {
                    result += this->vectorToString(*element) + ", ";
                }
            }
            result = result.substr(0, result.size() - 2);
            result += "}";
            return result;
        };
    };
    
    class ArrayString {
        std::vector<std::unique_ptr<std::string>> vector;
        std::vector<std::unique_ptr<std::vector<std::unique_ptr<std::string>>>> matrix;
        int dimensions;

        public:
        ArrayString(runtime::VarLen32 array, int dimensions) : dimensions(dimensions) {
            std::string content = array.str();
            if (dimensions == 1) {
                this->vector = std::vector<std::unique_ptr<std::string>>();
                this->toVector(content, this->vector);
            } else if (dimensions == 2) {
                this->matrix = std::vector<std::unique_ptr<std::vector<std::unique_ptr<std::string>>>>();
                this->toMatrix(content);
            } else {
                throw std::runtime_error("More than 2 dimensions are currently not supported");
            }
        };

        runtime::VarLen32 toString() {
            std::string result;

            if (this->dimensions == 2) {
                result = this->matrixToString();
            } else {
                result = this->vectorToString(this->vector);
            }

            char* data = new char[result.length()];           
            memcpy(data, result.data(), result.length());     
            return runtime::VarLen32((uint8_t*) data, result.length());
        };

        private:
        void toVector(std::string array, std::vector<std::unique_ptr<std::string>>& container) {
            // Delete outer brackets ({})
            array = array.substr(1, array.size() - 2);
            std::stringstream stringStream(array);
            std::string singleValue;
            bool isElement = false;
            while(std::getline(stringStream, singleValue, '"')) {
                if (isElement) {
                    container.push_back(std::make_unique<std::string>(singleValue));
                } else {
                    singleValue.erase(std::remove_if(singleValue.begin(), singleValue.end(), ::isspace), singleValue.end());
                    // Proof if a single element is the null-value
                    if (singleValue.find("Null") != std::string::npos || singleValue.find("NULL") != std::string::npos|| singleValue.find("null") != std::string::npos){
                        container.push_back(nullptr);
                    }
                    // Proof if some non-string elements were skipped (e.g. {"Hello World", 1, "Test"} whereby '1' has no quotation marks) 
                    else if (singleValue.size() > 2) {
                        throw std::runtime_error("The array value - " + singleValue + " - does not have the correct type");
                    }
                }
                isElement = !isElement;
            }
        };

        void toMatrix(std::string array) {
            // Remove outer brackets ({})
            array = array.substr(1, array.size() - 2);
            std::stringstream stringStream(array);
            std::string singleVector;
            // Iterate over each vector element of the matrix
            while (std::getline(stringStream, singleVector, '}')) {
                std::unique_ptr<std::vector<std::unique_ptr<std::string>>> vector = std::make_unique<std::vector<std::unique_ptr<std::string>>>();
                auto startIndex = singleVector.find('{');
                // If the opening bracket is missing, the dimension of the input is not valid
                if (startIndex == std::string::npos) {
                    throw std::runtime_error("The elements of the array does not correspond with the specification of the dimensions number");
                }
                // It is possible that there is a NULL value that was skipped (has no {})
                if (singleVector.find("NULL") < startIndex || singleVector.find("Null") < startIndex || singleVector.find("null") < startIndex) {
                    this->matrix.push_back(nullptr);
                }
                // Remove every character before '{'
                singleVector = singleVector.substr(startIndex, singleVector.size() - 1);
                // Add the deleted "}" from the "getline" function
                singleVector = singleVector + '}';
                // Call the toVector function for a std::string
                this->toVector(singleVector, *vector);
                this->matrix.push_back(std::move(vector)); 
            }
        };

        std::string vectorToString(std::vector<std::unique_ptr<std::string>>& container) {
            std::string result = "{";
            for (auto& element : container) {
                if (element == nullptr) {
                    result += "null, ";
                } else {
                    result += *element + ", ";
                }
            }
            result = result.substr(0, result.size() - 2);
            result += "}";
            return result;
        };

        std::string matrixToString() {
            std::string result = "{";
            for (auto& element : this->matrix) {
                if (element == nullptr) {
                    result += "null, ";
                } else {
                    result += this->vectorToString(*element) + ", ";
                }
            }
            result = result.substr(0, result.size() - 2);
            result += "}";
            return result;
        };
    };

    struct ArrayRuntime{

    };
    
} // namespace runtime

#endif // RUNTIME_ARRAYRUNTIME_H