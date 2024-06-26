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
#include <functional>
#include <stdexcept>

namespace runtime
{
    struct ArrayRuntime{
        /**
        * This function concatenates two arrays. It will be able to concatenate a vector (array2) into a matrix (array1) and vice versa add all
        * single elements to the vector. If both arrays have different types or one of the given arrays as VarLen32 cannot be converted 
        * to a Array object, it will throw an runtime_error. 
        * @param array1     The array which should be extended with more elements
        * @param dim1       The dimension value of the first array
        * @param type1      The type of the elements in the first array
        * @param array2     The array which should be moved to the first array
        * @param dim2       The dimension value of the second array
        * @param type2      The type the elements in the second array
        */
        static runtime::VarLen32 concat(runtime::VarLen32 array1, int dim1, runtime::VarLen32 type1, runtime::VarLen32 array2, int dim2, runtime::VarLen32 type2);

        static runtime::VarLen32 getRange(runtime::VarLen32 array, int dim, runtime::VarLen32 type, int start, int stop);
        static runtime::VarLen32 getEntry(runtime::VarLen32 array, int dim, runtime::VarLen32 type, int index);
    };

    /**
     * This class represents an array for any type (e.g. int32_t, int64_t, float, etc.)
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
        /**
         * Construct an Array object.
         * @param array         The content as string inside a VarLen32 object
         * @param dimensions    How many dimensions the array has
         * @param func          A function which can convert an element to the corresponding type -T
         * @throw std::runtime_error    - If the containing array does not match with the given dimensions value of if the
         *                                dimensions value is larger than 2. If an element could not converted to type T 
         *                                with the given convertion function.
         * 
         * 
         */
        Array(runtime::VarLen32 array, int dimensions, std::function<T(std::string)> func) : dimensions(dimensions) {
            std::string content = array.str();
            this->vector = std::vector<std::unique_ptr<T>>();
            this->matrix = std::vector<std::unique_ptr<std::vector<std::unique_ptr<T>>>>();
            // According to the given dimension create the corresponding object
            if (dimensions == 1) {
                this->toVector(content, this->vector, func);
            } else if (dimensions == 2) {
                this->toMatrix(content, this->matrix, func);
            } else {
                throw std::runtime_error("More than 2 dimensions are currently not supported");
            }
        };

        /**
         * This method converts the given array into a string (stored as VarLen32 object) 
         * @param func  The function which should be used to convert an element of type T to a std::string
         */
        runtime::VarLen32 toString(std::function<std::string(T)> func) {
            std::string result;

            if (this->dimensions == 2) {
                result = this->matrixToString(this->matrix, func);
            } else {
                result = this->vectorToString(this->vector, func);
            }

            // These are necessary steps to create a VarLen32 object
            char* data = new char[result.length()];           
            memcpy(data, result.data(), result.length());     
            return runtime::VarLen32((uint8_t*) data, result.length());
        };

        /**
         * This method adds the elements of the given array.
         * @param secondArray   A pointer to the Array object which elements should be added 
         */
        void concat(runtime::Array<T>* secondArray) {
            // Add a vector to this matrix
            if (this->dimensions > secondArray->getDimensions()){
                auto vector = std::make_unique<std::vector<std::unique_ptr<T>>>(std::move(*(secondArray->getVector())));
                this->matrix.push_back(std::move(vector));
            // Add a matrix to this vector
            } else if (this->dimensions < secondArray->getDimensions()) {
                for (auto& vector : *(secondArray->getMatrix())){
                    // Null-values of a matrix will be ignored
                    if (vector != nullptr) {
                        for (auto& element : *vector) {
                            this->vector.push_back(std::move(element));
                        }
                    }
                }
            } else {
                // If merging 2 matrices
                if (this->dimensions == 2) {
                    for (auto& element : *(secondArray->getMatrix())) {
                        this->matrix.push_back(std::move(element));
                    }
                // If merging 2 vectors
                } else {
                    for (auto& element : *(secondArray->getVector())) {
                        this->vector.push_back(std::move(element));
                    }
                }
            }
        };

        /**
         * This method returns a pointer to its vector
         */
        std::vector<std::unique_ptr<T>>* getVector(){
            return &this->vector;
        };

        /**
         * This method returns a pointer to its matrix
         */
        std::vector<std::unique_ptr<std::vector<std::unique_ptr<T>>>>* getMatrix(){
            return &this->matrix;
        };

        /**
         * This method returns its dimension value
         */
        int getDimensions(){
            return this->dimensions;
        }

        /**
         * This static method converts a std::string into a int32_t
         */
        static int32_t stringToInt32(std::string value) {
            return std::stoi(value);
        };


        /**
         * This static method converts a std::string into a int64_t
         */
        static int64_t stringToInt64(std::string value) {
            return std::stoll(value);
        };

        /**
         * This static method converts a std::string into a float
         */
        static float stringToFloat(std::string value) {
            return std::stof(value);
        };

        /**
         * This static method converts a std::string into a double
         */
        static double stringToDouble(std::string value) {
            return std::stod(value);
        };

        /**
         * This static method converts a std::string into a std::string
         */
        static std::string stringToArrayString(std::string value) {
            return value;
        };

        /**
         * This static method converts a primitive numeric value into a std::string
         */
        template<typename R>
        static std::string numericToString(R value) {
            return std::to_string(value);
        };

        /**
         * This static method converts a std::string into a std::string, but with adding quotation marks
         */
        static std::string arrayStringToString(std::string value) {
            return '"' + value + '"';
        };

        private:

        /**
         * This method converts the given string into a std::vector containing std::unique_ptr to the corresponding elements. If the array contains 
         * null values, they will be represented as nullptr. 
         * @param array         The string which should be converted
         * @param container     A reference to the vector where the elements should be added
         * @param func          A function which is used to convert the elements to the corresponding type T.
         * @throws runtime_error    - If the string contains elements which cannot be converted to the specified type
         */
        void toVector(std::string array, std::vector<std::unique_ptr<T>>& container, std::function<T(std::string)> func) {
            // Delete outer brackets ({})
            array = array.substr(1, array.size() - 2);
            std::stringstream stringStream(array);
            std::string singleValue;
            char delimiter = std::is_same<T, std::string>::value ? '"' : ',';
            bool isElement = false;
            while(std::getline(stringStream, singleValue, delimiter)) {
                // If one of these null-strings occur, add nullptr
                if (singleValue.find("Null") != std::string::npos || 
                    singleValue.find("NULL") != std::string::npos || 
                    singleValue.find("null") != std::string::npos){
                    container.push_back(nullptr);
                // Otherwise convert value to specified type and add it
                } else {
                    try {
                        if (std::is_same<T, std::string>::value && isElement) {
                            T value = func(singleValue);
                            container.push_back(std::make_unique<T>(value));
                        } else if (!std::is_same<T, std::string>::value) {
                            singleValue.erase(std::remove_if(singleValue.begin(), singleValue.end(), ::isspace), singleValue.end());
                            T value = func(singleValue);
                            container.push_back(std::make_unique<T>(value));
                        }
                    } catch (const std::invalid_argument& exception) {
                        throw std::runtime_error(singleValue + " cannot be converted");
                    }
                }
                // This is necessary if T == std::string, because of using other delimiter
                isElement = !isElement;
            }
        };


        /**
         * This methods converts a string into a matrix (std::vector with std::unique_ptr containing std::vector). If the array contains 
         * null values, they will be represented as nullptr. If the string contains elements which cannot be converted to the specified type a 
         * runtime_error will be thrown.
         * @param array     The string which should be converted
         * @param container The container in which the elements should be added
         * @param func      The function which should be used to convert an element to type T
         */
        void toMatrix(std::string array, std::vector<std::unique_ptr<std::vector<std::unique_ptr<T>>>>& container, std::function<T(std::string)> func) {
            // Remove outer brackets ({})
            array = array.substr(1, array.size() - 2);
            std::stringstream stringStream(array);
            std::string singleVector;
            // Iterate over each vector element of the matrix
            while (std::getline(stringStream, singleVector, '}')) {
                auto startIndex = singleVector.find('{');
                // It is possible that there is a NULL value that was skipped (has no {})
                if (singleVector.find("NULL") < startIndex || singleVector.find("Null") < startIndex || singleVector.find("null") < startIndex) {
                    container.push_back(nullptr);
                    // Proof if the find null value is the last entry of that matrix
                    if (startIndex == std::string::npos) {
                        break;
                    }
                }
                // If the opening bracket is missing, the dimension of the input is not valid
                if (startIndex == std::string::npos) {
                    throw std::runtime_error(array + " has not expected dimension of " + std::to_string(this->dimensions));
                }
                std::unique_ptr<std::vector<std::unique_ptr<T>>> vector = std::make_unique<std::vector<std::unique_ptr<T>>>();
                // Remove every character before '{'
                singleVector = singleVector.substr(startIndex, singleVector.size());
                // Add the deleted "}" from the "getline" function
                singleVector = singleVector + '}';
                this->toVector(singleVector, *vector, func);
                container.push_back(std::move(vector));                 
            }
        };

        /**
         * This method converts a vector object into a string.
         * @param container     The std::vector which should be converted into a string
         * @param func          The function which should be used for conversion
         */
        std::string vectorToString(std::vector<std::unique_ptr<T>>& container, std::function<std::string(T)> func) {
            std::string result = "{";
            for (auto& element : container) {
                if (element == nullptr) {
                    result += "null, ";
                } else {
                    result += func(*element) + ", ";
                }
            }
            result = result.substr(0, result.size() - 2);
            result += "}";
            return result;
        };

        /**
         * This method converts a matrix into a string.
         * @param container     The std::vector which should be converted into a string
         * @param func          The function which should be used for conversion
         */
        std::string matrixToString(std::vector<std::unique_ptr<std::vector<std::unique_ptr<T>>>>& container, std::function<std::string(T)> func) {
            std::string result = "{";
            for (auto& element : container) {
                if (element == nullptr) {
                    result += "null, ";
                } else {
                    result += this->vectorToString(*element, func) + ", ";
                }
            }
            result = result.substr(0, result.size() - 2);
            result += "}";
            return result;
        };
    };
    
} // namespace runtime

#endif // RUNTIME_ARRAYRUNTIME_H