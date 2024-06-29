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
        * @returns A modified array wrapped up as string in a VarLen32 object
        * @throws std::runtime_error   -    If one of the given arrays could not be created according to the given str, dim and type parameters. 
        */
        static runtime::VarLen32 concat(runtime::VarLen32 array1, int dim1, runtime::VarLen32 type1, runtime::VarLen32 array2, int dim2, runtime::VarLen32 type2);

        /**
         * This function allows to pick up some elements of a given array in a specific range.
         * @param str       The intial array
         * @param dim       The amount of dimension of the given array
         * @param type      The type of the elements from the given array
         * @param start     The start index or start value of the range specification
         * @param stop      The stop index or end value of the range specification
         * @returns A modified array wrapped up as string in a VarLen32 object
         * @throws std::runtime_error   -   If the start or stop indices are wrongly specified or the array could not be created according to the 
         *                                  given str, dim and type parameters. 
         */
        static runtime::VarLen32 getRange(runtime::VarLen32 str, int dim, runtime::VarLen32 type, int start, int stop);

        /**
         * This function allows to pick up a single element of a given array.
         * @param str       The intial array
         * @param dim       The amount of dimension of the given array
         * @param type      The type of the elements from the given array
         * @param index     The index of the element which should be returned
         * @returns A modified array wrapped up as string in a VarLen32 object
         * @throws std::runtime_error   -   If the index is wrongly specified or the array could not be created according to the 
         *                                  given str, dim and type parameters. 
         */
        static runtime::VarLen32 getEntry(runtime::VarLen32 str, int dim, runtime::VarLen32 type, int index);

        /**
         * This function returns a string of dimension values (for array). E.g. for a one dimensional array it will be a single entry, but for a
         * two dimensional array it will be n entries, depending on the size.
         * @param str       The initial array
         * @param dim       The amount of dimension of the given array
         * @param type      The type of the elements from the given array
         * @returns A VarLen32 object containing every dimension information
         * @throws std::runtime_error   -   If the array could not be created according to the given str, dim and type parameters.
         */
        static runtime::VarLen32 getDimensions(runtime::VarLen32 str, int dim, runtime::VarLen32 type);

        static runtime::VarLen32 getCardinality(runtime::VarLen32 str, int dim, runtime::VarLen32 type);
    };




    /**
       This class represents an array structure with abitrary dimensions. The type ```T``` defines which type the corresponding elements should have.
     */
    template<typename T>
    class ArrayF {

        /**
           This class represents a single array element of type ```R```. If an instance does not contain any element it will be a ```null``` value.
         */
        template<typename R>
        class ArrayElem {
            using TypeCast = std::function<R(std::string)>;
            using StringCast = std::function<std::string(R)>;

            // The actual data
            R value;
            // If the instance represents a null value
            bool isNull;
            // A function to cast a string to the corresponding type R
            TypeCast castToType;
            // A function to cast a value of type R to a string
            StringCast castToString;

            public:
            /**
             * This constructor creates an instance of ```ArrayElem```.
             * @param value             The actual data which should be stored in this object
             * @param typeCast          A function which allows to convert a ```std::string``` to ```R```
             * @param stringCast        A function which allows to convert a value of type ```R``` to a ```std::string```
             * @note                    If the given ```value``` could not be converted to type ```R```, a ```std::runtime_error``` will be thrown
             */
            ArrayElem(std::string value, TypeCast typeCast, StringCast stringCast) : isNull(false), castToType(typeCast), castToString(stringCast) {
                try {
                    this->value = this->castToType(value);
                } catch (std::invalid_argument error) {
                    throw std::runtime_error("The value -" + value + "- could not be converted. Check the dimension of your input or the entered values");
                }
            }

            /**
               This constructor creates an empty instance of ```ArrayElem``` which represents a ```null``` value.
             */
            ArrayElem() : isNull(true){}

            /**
             * This method converts the content of a ```ArrayElem``` object to a string
             */
            std::string toString(){
                if (this->isNull){
                    return "null, ";
                } else {
                    return this->castToString(this->value) + ", ";
                }
            }
        };

        /**
           This class represents a list of array entries. The following two options can be distinguished: ```{1,2,3,4}``` with a list of 
           ```ArrayElem``` objects or ```{{1,2,3,4},{5,6,7,8}}``` with ```ArrayList``` objects.
         */
        template<typename R>
        class ArrayList {
            using TypeCast = std::function<R(std::string)>;
            using StringCast = std::function<std::string(R)>;

            // A vector with single elements
            std::vector<ArrayElem<R>> elements;
            // A vector with more complex structure
            std::vector<ArrayList<R>> container;
            // The dimension of the object in the array
            int32_t dimension;
            // If the content is a null value
            bool isNull;

            public:
            /**
             *  This constructor creates a ```ArrayList``` instance, containing the array structure from ```array``` in a more accessible form.
             *  @param array         The array as string which should be converted to a more accessible array structure. Be aware that each entry must be
             *                       defined with ```{}``` brackets
             *  @param dimension     Defines how many dimensions the array has. Should fit with the given array structure from the ```array``` parameter
             *  @param typeCast      A function which allows to convert a ```std::string``` to ```R```
             *  @param stringCast    A function which allows to convert a value of type ```R``` to a ```std::string``` 
             *  @note                If the entered dimension does not correspond to the dimension structure of the array parameter, a ```std::runtime_error``` 
             *                       will be thrown, e.g. ``` array = '{1,2,3,4}' ``` and ``` dimension = 2 ```
             *  @note                If a entered value cannot be converted to the specified type then a ```std::runtime_error``` will be thrown
             *                       (e.g. 'Hello World' -> int)
             *  @note                If the given array parameter does not correspond to the defined array signatur a ```std::runtime::error``` will be thrown,
             *                       e.g. ```array = [1,2,3,4]```
             */
            ArrayList(std::string array, int32_t dimension, TypeCast typeCast, StringCast stringCast) : dimension(dimension), isNull(false) {
                if (dimension == 1) {
                    this->setElements(array, typeCast, stringCast);
                } else {
                    this->setContainer(array, typeCast, stringCast);
                }
            }

            /**
             * This constructor creates an empty instance of ```ArrayList``` which represents a ```null``` value.
             */
            ArrayList() : isNull(true) {}

            /**
             * This method converts the content of a ```ArrayList``` object to a string 
             */
            std::string toString(){
                if (this->isNull){
                    return "null, ";
                } else {
                    std::string result = "{";
                    // If container contains single elements
                    if (this->dimension == 1) {
                        for (auto& element : this->elements) {
                            result += element.toString();
                        }
                        // Delete the last characters which are ', '
                        result = result.substr(0, result.size() - 2);
                    // If container contains more complex structures
                    } else {
                        for (auto& element : this->container) {
                            result += element.toString();
                        }
                        // Delete the last character which is ','
                        result = result.substr(0, result.size() - 1);
                    }
                    result += "},";
                    return result;
                }
            }

            private:
            /**
             * This method disassembles the given ```array``` into a corresponding array structure. It creates for every element
             * a new ```ArrayList``` object and push it to the ```container``` attribute.
             * @param array          The array as string which should be converted to a more accessible array structure
             * @param typeCast       A function which allows to convert a ```std::string``` to ```R```
             * @param stringCast     A function which allows to convert a value of type ```R``` to a ```std::string```
             * @note                 If the entered dimension does not correspond to the dimension structure of the array parameter, a ```std::runtime_error``` 
             *                       will be thrown, e.g. ``` array = '{1,2,3,4}' ``` and ``` dimension = 2 ```
             * @note                 If a entered value cannot be converted to the specified type then a ```std::runtime_error``` will be thrown
             *                       (e.g. 'Hello World' -> int)
             * @note                 If the given array parameter does not correspond to the defined array signatur a ```std::runtime::error``` will be thrown,
             *                       e.g. ```array = [1,2,3,4]```
             */
            void setContainer(std::string array, TypeCast typeCast, StringCast stringCast) {
                this->container = std::vector<ArrayList<R>>();
                // Proof if the content is a null value
                if (!this->isInputNull(array)){
                    // Counts, how many '{' have been encounterd which has not been closed
                    int32_t openEntries = -1; // -1 = does not found any '{'
                    // Index of the first character of an array element (skip the first '{')
                    int32_t startIndex = array.find('{') + 1;
                    // Iterate over every character from the array parameter
                    for (int32_t index = startIndex; index < array.size(); index++){
                        // If the corresponding '}' of an element has been identified
                        if (openEntries == 0){
                            // Create a new ArrayList with the corresponding substring and a lower dimension
                            std::string data = array.substr(startIndex, index - startIndex);
                            ArrayList<R> subContainer(data, this->dimension - 1, typeCast, stringCast);
                            this->container.push_back(subContainer);
                            openEntries = -1;
                            startIndex = index;
                        }
                        // Proof every character
                        switch (array[index]){
                        case '{':
                            // If no starting '{' could be found, but more than 1 character between startIndex and index (can be null value)
                            if (openEntries == -1 && index - startIndex > 1) {
                                // Proof if it is a null value
                                if (this->isInputNull(array.substr(startIndex, index - startIndex))){
                                    ArrayList<R> subContainer;
                                    this->container.push_back(subContainer);
                                }
                            }
                            // Reset startIndex if necessary (ensure that startIndex will point to '{')
                            startIndex = openEntries == -1 ? index : startIndex;
                            // Increment openEntries (special case for first '{')
                            openEntries = openEntries == -1 ? 1 : openEntries + 1;
                            break;
                        case '}':
                            openEntries--;
                            // Proof if a null value is between last entry and end of array (if last element is a null value)
                            if (openEntries < 0 && this->isInputNull(array.substr(startIndex + 1, index - startIndex - 1))) {
                                ArrayList<R> subContainer;
                                this->container.push_back(subContainer);
                            }
                            break;
                        }
                    }
                    // If no elements could be encountered, means that the array would probably have a false specification (dim(array) < this->dimension).
                    if (this->container.size() == 0){
                        throw std::runtime_error("The entered array instance has a smaller dimension than specified. Pleasre ensure that your instance should have a dimension of: " + std::to_string(this->dimension)); 
                    }
                } else {
                    this->isNull = true;
                }
            }

            /**
             * This method disassembles the given ```array``` into a corresponding array structure. It creates for every element
             * a new ```ArrayElem``` object and push it to the ```elements``` attribute.
             * @param array          The array as string which should be converted to a more accessible array structure
             * @param typeCast       A function which allows to convert a ```std::string``` to ```R```
             * @param stringCast     A function which allows to convert a value of type ```R``` to a ```std::string```
             * @note                 If the entered dimension does not correspond to the dimension structure of the array parameter, a ```std::runtime_error``` 
             *                       will be thrown, e.g. ``` array = '{1,2,3,4}' ``` and ``` dimension = 2 ```
             * @note                 If a entered value cannot be converted to the specified type then a ```std::runtime_error``` will be thrown
             *                       (e.g. 'Hello World' -> int)
             * @note                 If the given array parameter does not correspond to the defined array signatur a ```std::runtime::error``` will be thrown,
             *                       e.g. ```array = [1,2,3,4]```
             */
            void setElements(std::string array, TypeCast typeCast, StringCast stringCast) {
                this->elements = std::vector<ArrayElem<R>>();
                // Proof if the content is a null value
                if (!this->isInputNull(array)){
                    // Index of the first character of an array element (skip the first '{')
                    int32_t startIndex = array.find('{') + 1;
                    // Iterate over every character from the array parameter
                    for (int32_t index = startIndex; index < array.size(); index++){
                        // Proof every character
                        switch (array[index]){
                        case '}':
                        case ',':
                            // Get the element from the string
                            std::string data = array.substr(startIndex, index - startIndex);
                            ArrayElem<R> element;
                            // Proof if it is null
                            if (this->isInputNull(data)){
                                element = ArrayElem<R>();
                            } else {
                                element = ArrayElem<R>(data, typeCast, stringCast);
                            }
                            this->elements.push_back(element);
                            startIndex = index + 1;
                            break;
                        }
                    }
                    // If no elements could be encountered, means that the array would probably have a false specification (dim(array) < this->dimension).
                    if (this->elements.size() == 0){
                        throw std::runtime_error("The entered array instance has a smaller dimension than specified. Pleasre ensure that your instance should have a dimension of: " + std::to_string(this->dimension));  
                    }
                } else {
                     this->isNull = true;
                }
            }

            /**
             * This method proofs if the given string is a ```null``` value.
             * @param entry         The string which should be checked.
             * @return              ```True```, if string is a null value, otherwise ```false```
             */
            bool isInputNull(std::string entry){
                // A function which is needed to remove every ',' and ' ' from the entry
                auto removeChar = [](char c){
                    switch (c){
                    case ' ':
                    case ',':
                        return true;
                    default:
                        return false;
                    }
                };
                // Remove characters
                entry.erase(std::remove_if(entry.begin(), entry.end(), removeChar), entry.end());
                // Proof if null value
                if (entry == "NULL" || entry == "Null" || entry == "null") {
                    return true;
                }
                return false;
            }
        };

        using TypeCast = std::function<T(std::string)>;
        using StringCast = std::function<std::string(T)>;

        // The array structure
        ArrayList<T> array;

        public:

        /**
         * This constructor creates a new array instance based on the given array parameter.
         * 
         * @param array         The array as string which should be converted to a more accessible array structure. Be aware that each entry must be
         *                      defined with ```{}``` brackets
         * @param dimension     Defines how many dimensions the array has. Should fit with the given array structure from the array parameter
         * @param typeCast      A function which allows to convert a string to the defined type ```T``` (also necessary for string arrays)
         * @param stringCast    A function which allows to cast a value of type ```T``` into a string (also necessay for string arrays)
         * @note                If the entered dimension does not correspond to the dimension structure of the array parameter, a ```std::runtime_error``` 
         *                      will be thrown, e.g. ``` array = '{1,2,3,4}' ``` and ``` dimension = 2 ```
         * @note                If a entered value cannot be converted to the specified type then a ```std::runtime_error``` will be thrown
         *                      (e.g. 'Hello World' -> int)
         * @note                If the given array parameter does not correspond to the defined array signatur a ```std::runtime::error``` will be thrown,
         *                      e.g. ```array = [1,2,3,4]```
         */
        ArrayF(std::string array, int32_t dimension, TypeCast typeCast, StringCast stringCast) : array(ArrayList<T>(array, dimension, typeCast, stringCast)) {}

        /**
         * This method converts the complete array structure back to a string.
         * @return              A ```VarLen32``` object which includes the array as string.
         */
        runtime::VarLen32 toString(){
            std::string result = this->array.toString();
            // Delete the last character which will be a ','
            result = result.substr(0, result.size() - 1);
            
            // These are necessary steps to create a VarLen32 object
            char* data = new char[result.length()];           
            memcpy(data, result.data(), result.length());     
            return runtime::VarLen32((uint8_t*) data, result.length());
        }

    };

    /**
     * This class is an abstract class which includes all cast functions for an ```Array``` construction.
     */
    class TypeCasts {
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
        static std::string stringToString(std::string value) {
            return value;
        };

        /**
         * This static method converts a primitive numeric value into a std::string
         */
        template<typename R>
        static std::string numericToString(R value) {
            return std::to_string(value);
        };
    };






















































    /**********************************************************************************************************************************
     * 
     * 
     * 
     *                                                      DEPRECATED (will be replaced with above code)
     * 
     * 
     * 
     ***********************************************************************************************************************************/


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
            } else if (dimensions < 1) {
                throw std::runtime_error("That's an invalid dimension. Should be at least 1");
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
            } else if (this->dimensions == 1) {
                result = this->vectorToString(this->vector, func);
            } else {
                // This case is called if the array consists only with a single value
                result = this->vector[0] == nullptr ? "null" : func(*(this->vector[0]));
                // This statement deletes duplicated quotation marks which happens if a single std::string value is returned
                if (result.find("") != std::string::npos){
                    result = result.substr(1, result.length() - 1);
                }
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
            if (this->dimensions > secondArray->getDimension()){
                auto vector = std::make_unique<std::vector<std::unique_ptr<T>>>(std::move(*(secondArray->getVector())));
                this->matrix.push_back(std::move(vector));
            // Add a matrix to this vector
            } else if (this->dimensions < secondArray->getDimension()) {
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
         * This method deletes every element in the array except the ones defined in the given range (including start and end).
         * @param start     Index which represents the first value in the resulting array
         * @param stop      Index which represents the last value in the resulting array
         * @throws std::runtime_error - If one of the indices is out of bounds of the array or if the start index is larger than the stop index
         */
        void setToRange(int start, int stop) {
            int length = this->dimensions == 1 ? this->vector.size() : this->matrix.size();
            if (start >= length || start < 0) {
                throw std::runtime_error("Start-index out of bounds");
            }
            if (stop >= length || stop < 0) {
                throw std::runtime_error("Stop-index out of bounds");
            }
            if (start >= stop) {
                throw std::runtime_error("First index - " + std::to_string(start) + " - should be smaller than the second index - " + std::to_string(stop));
            }
            // Loop over every element and delete those which cannot be assigned to the given index-range
            for (int index = 0; index < length; index++){
                if (index < start || index > stop) {
                    if (this->dimensions == 1) {
                        this->vector.erase(this->vector.begin() + index);
                    } else {
                        this->matrix.erase(this->matrix.begin() + index);
                    }
                    index--;
                    length--;
                }
            }
        };

        /**
         * This method changes the array, so that only the element with the given index remains.
         * @param element       Index which represents the position of the element which should remain
         * @throws std::runtime_error - If the given index is out of bounds of the array
         */
        void setToElement(int element){
            int length = this->dimensions == 1 ? this->vector.size() : this->matrix.size();
            if (element >= length || element < 0) {
                throw std::runtime_error("Index out of bounds");
            }
            if (this->dimensions == 2) {
                this->vector = std::move(*(this->matrix[element]));
            } else {
                auto newVector = std::vector<std::unique_ptr<T>>();
                newVector.push_back(std::move(this->vector[element]));
                this->vector = std::move(newVector);
            }
            this->dimensions--;
        };

        /**
         * This method returns a VarLen32 object which stores a string with a list of dimensions entries. Each entry corresponds to a 
         * single one dimensional array.
         */
        runtime::VarLen32 getDimensions(){
            std::string result;
            if (this->dimensions == 2) {
                result = this->getMatrixDimension(this->matrix);
            } else {
                result = this->getVectorDimension(this->vector);
            }

            // These are necessary steps to create a VarLen32 object
            char* data = new char[result.length()];           
            memcpy(data, result.data(), result.length());     
            return runtime::VarLen32((uint8_t*) data, result.length());
        };

        runtime::VarLen32 getCardinality(){
            int size = 0;
            if (this->dimensions == 2) {
                for (auto& element : this->matrix) {
                    if (element == nullptr) {
                        size++;
                    } else {
                        size += element->size();
                    }
                }
            } else {
                size = this->vector.size();
            }
            std::string result = std::to_string(size);

            // These are necessary steps to create a VarLen32 object
            char* data = new char[result.length()];           
            memcpy(data, result.data(), result.length());     
            return runtime::VarLen32((uint8_t*) data, result.length());
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
        int getDimension(){
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

        std::string getVectorDimension(std::vector<std::unique_ptr<T>>& container) {
            return "[1:" + std::to_string(container.size()) + "]";
        };

        std::string getMatrixDimension(std::vector<std::unique_ptr<std::vector<std::unique_ptr<T>>>>& container) {
            std::string result = "";
            for (auto& element : container) {
                if (element == nullptr) {
                    result += "[1:1]";
                } else {
                    result += this->getVectorDimension(*element);
                }
            }
            return result;
        };
    };
    
} // namespace runtime

#endif // RUNTIME_ARRAYRUNTIME_H