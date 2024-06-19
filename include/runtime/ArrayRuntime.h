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

    /**
     * This class represents an array for any primitive numeric type (e.g. int32_t, int64_t, float, etc.)
     */
    template<typename T>
    class Array {
        // Attribute which stores a single vector
        std::vector<std::unique_ptr<T>> vector;
        // Attribute which stores a single matrix
        std::vector<std::unique_ptr<std::vector<std::unique_ptr<T>>>> matrix;
        // Attribute which decides if result is a vector or a matrix
        int dimensions;

        public:
        Array(runtime::VarLen32 array, int dimensions) : dimensions(dimensions) {
            std::string content = array.str();
            // According to the given dimension create the corresponding object
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

        /*
        * This method converts the given array into a string (stored as VarLen32 object) 
        */
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

        /**
         * This method converts the given string into a std::vector containing std::unique_ptr to the corresponding elements. If the array contains 
         * null values, they will be represented as nullptr. If the string contains elements which cannot be converted to the specified type a 
         * runtime_error will be thrown.
         * @param array         The string which should be converted
         * @param container     A reference to the vector where the elements should be added
         */
        void toVector(std::string array, std::vector<std::unique_ptr<T>>& container) {
            // Delete spaces if content is not std::string
            array.erase(std::remove_if(array.begin(), array.end(), ::isspace), array.end());
            // Delete outer brackets ({})
            array = array.substr(1, array.size() - 2);
            std::stringstream stringStream(array);
            std::string singleValue;
            while(std::getline(stringStream, singleValue, ',')) {
                // If one of these null-strings occur, add nullptr
                if (singleValue.find("Null") != std::string::npos || 
                    singleValue.find("NULL") != std::string::npos || 
                    singleValue.find("null") != std::string::npos){
                    container.push_back(nullptr);
                // Otherwise convert value to specified type and add it
                } else {
                    T value = this->castToNumber(singleValue);
                    container.push_back(std::make_unique<T>(value));
                }
            }
        };

        /**
         * This method converts a string value to its specified type. If the value cannot be converted it will throw a runtime_error
         * @param element   The element which should be converted
         */
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


        /**
         * This methods converts a string into a matrix (std::vector with std::unique_ptr containing std::vector). If the array contains 
         * null values, they will be represented as nullptr. If the string contains elements which cannot be converted to the specified type a 
         * runtime_error will be thrown.
         * @param array     The string which should be converted
         */
        void toMatrix(std::string array) {
            // Remove all spaces
            array.erase(std::remove_if(array.begin(), array.end(), ::isspace), array.end());
            // Remove outer brackets ({})
            array = array.substr(1, array.size() - 2);
            std::stringstream stringStream(array);
            std::string singleVector;
            // Iterate over each vector element of the matrix
            while (std::getline(stringStream, singleVector, '}')) {
                auto startIndex = singleVector.find('{');
                // It is possible that there is a NULL value that was skipped (has no {})
                if (singleVector.find("NULL") < startIndex || singleVector.find("Null") < startIndex || singleVector.find("null") < startIndex) {
                    this->matrix.push_back(nullptr);
                    // Proof if the find null value is the last entry of that matrix
                    if (startIndex == std::string::npos) {
                        break;
                    }
                }
                // If the opening bracket is missing, the dimension of the input is not valid
                if (startIndex == std::string::npos) {
                    throw std::runtime_error("The elements of the array does not correspond with the specification of the dimensions number");
                }
                std::unique_ptr<std::vector<std::unique_ptr<T>>> vector = std::make_unique<std::vector<std::unique_ptr<T>>>();
                // Remove every character before '{'
                singleVector = singleVector.substr(startIndex, singleVector.size());
                // Add the deleted "}" from the "getline" function
                singleVector = singleVector + '}';
                this->toVector(singleVector, *vector);
                this->matrix.push_back(std::move(vector));                 
            }
        };

        /**
         * This method converts a vector object into a string.
         * @param container     The std::vector which should be converted into a string
         */
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

        /**
         * This method converts a matrix into a string.
         */
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
    
    /**
     * This class represents an array for the std::string type
     */
    class ArrayString {
        // Attribute which stores a single vector
        std::vector<std::unique_ptr<std::string>> vector;
        // Attribute which stores a single matrix
        std::vector<std::unique_ptr<std::vector<std::unique_ptr<std::string>>>> matrix;
        // Attribute which decides if result is a vector or a matrix
        int dimensions;

        public:
        ArrayString(runtime::VarLen32 array, int dimensions) : dimensions(dimensions) {
            std::string content = array.str();
            // According to the given dimension create the corresponding object
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

        /*
        * This method converts the given array into a string (stored as VarLen32 object) 
        */
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

        /**
         * This method converts the given string into a std::vector containing std::unique_ptr to the corresponding elements. If the array contains 
         * null values, they will be represented as nullptr. If the string contains elements which cannot be converted to the specified type a 
         * runtime_error will be thrown.
         * @param array         The string which should be converted
         * @param container     A reference to the vector where the elements should be added
         */
        void toVector(std::string array, std::vector<std::unique_ptr<std::string>>& container) {
            // Delete outer brackets ({})
            array = array.substr(1, array.size() - 2);
            std::stringstream stringStream(array);
            std::string singleValue;
            // Because the " symbol is the delimiter every second value is a valid string
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

        /**
         * This methods converts a string into a matrix (std::vector with std::unique_ptr containing std::vector). If the array contains 
         * null values, they will be represented as nullptr. If the string contains elements which cannot be converted to the specified type a 
         * runtime_error will be thrown.
         * @param array     The string which should be converted
         */
        void toMatrix(std::string array) {
            // Remove outer brackets ({})
            array = array.substr(1, array.size() - 2);
            std::stringstream stringStream(array);
            std::string singleVector;
            // Iterate over each vector element of the matrix
            while (std::getline(stringStream, singleVector, '}')) {
                std::unique_ptr<std::vector<std::unique_ptr<std::string>>> vector = std::make_unique<std::vector<std::unique_ptr<std::string>>>();
                auto startIndex = singleVector.find('{');
                // It is possible that there is a NULL value that was skipped (has no {})
                if (singleVector.find("NULL") < startIndex || singleVector.find("Null") < startIndex || singleVector.find("null") < startIndex) {
                    this->matrix.push_back(nullptr);
                    // Proof if the null value is the last entry
                    if (startIndex == std::string::npos) {
                        break;
                    }
                }
                // If the opening bracket is missing, the dimension of the input is not valid
                if (startIndex == std::string::npos) {
                    throw std::runtime_error("The elements of the array does not correspond with the specification of the dimensions number");
                }
                // Remove every character before '{'
                singleVector = singleVector.substr(startIndex, singleVector.size());
                // Add the deleted "}" from the "getline" function
                singleVector = singleVector + '}';
                // Call the toVector function for a std::string
                this->toVector(singleVector, *vector);
                this->matrix.push_back(std::move(vector)); 
            }
        };

        /**
         * This method converts a vector object into a string.
         * @param container     The std::vector which should be converted into a string
         */
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

        /**
         * This method converts a matrix into a string.
         */
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