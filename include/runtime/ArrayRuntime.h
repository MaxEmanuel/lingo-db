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
    class Array {

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
                // Remove every " " until first character occurs for a string
                if (std::is_same<R, std::string>::value) {
                    if (size_t startIndex = value.find('"')){
                        value = value.substr(startIndex, value.size());
                    }
                }
                try {
                    this->value = this->castToType(value);
                } catch (std::invalid_argument const&) {
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
                    return "null,";
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

            /**
             * This method joins two arrays together. It is also possible that the given ```array``` has a lower dimension than ```this```.
             * It will add the elements to the last entry
             * @param array         A reference to the ```ArrayList``` object which should be connected to ```this```
             * @note                If the dimension value of the parameter ```array``` is larger than the dimension value of ```this```,
             *                      it will throw an ```std::runtime_error```.
             */
            void concat(ArrayList<T>& array) {
                // If dim(this) > dim(array) push to last ArrayList entry
                if (this->dimension > array.getDimension()) {
                    this->container[this->container.size() - 1].concat(array);
                } else if (this->dimension < array.getDimension()){
                    throw std::runtime_error("The dimension -" + std::to_string(array.getDimension()) + "- is larger than the dimension -" + std::to_string(this->dimension) + "- of the array which should receive the elements");
                } else {
                    // If dimension is 1, then push elements to the elements attribute
                    if (this->dimension == 1) {
                        for (auto& element : array.getElements()) {
                            this->elements.push_back(element);
                        }
                    // If dimension is < 1, then push elements to the container attribute
                    } else {
                        for (auto& element: array.getContainer()) {
                            this->container.push_back(element);
                        }
                    }
                }
            }

            /**
             * This method returns the dimension value of the current ```ArrayList``` object.
             * @returns             An dimension value
             */
            int32_t getDimension(){
                return this->dimension;
            }

            /**
             * This method returns the elements attribute, a ```std::vector``` with a list of ```ArrayElem``` objects.
             * @returns             A reference to a the ```std::vector<ArrayElem<R>>```
             */
            std::vector<ArrayElem<R>>& getElements(){
                return this->elements;
            }

            /**
             * This method returns the container attribute, a ```std::vector``` with a list of ```ArrayList``` objects.
             * @returns             A reference to the ```std::vector<ArrayList<R>>```
             */
            std::vector<ArrayList<R>>& getContainer(){
                return this->container;
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
                    size_t startIndex = array.find('{') + 1;
                    // Iterate over every character from the array parameter
                    for (size_t index = startIndex; index < array.size(); index++){
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
                    size_t startIndex = array.find('{') + 1;
                    // Iterate over every character from the array parameter
                    for (size_t index = startIndex; index < array.size(); index++){
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
        Array(std::string array, int32_t dimension, TypeCast typeCast, StringCast stringCast) : array(ArrayList<T>(array, dimension, typeCast, stringCast)) {}

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

        /**
         * This method joins two arrays together. It is also possible that the given ```array``` has a lower dimension than ```this```.
         * It will add the elements to the last entry
         * @param array         A reference to the ```array``` object which should be connected to ```this```
         * @note                If the dimension value of the parameter ```array``` is larger than the dimension value of ```this```,
         *                      it will throw an ```std::runtime_error```.
         */
        void concat(Array<T>& array) {
            this->array.concat(array.getArray());
        }

        /**
         * This method returns a reference to the ```ArrayList``` attribute from this class
         * @return              A reference to the ```ArrayList``` value
         */
        ArrayList<T>& getArray(){
            return this->array;
        }

    };

    /**
     * This class is an abstract class which includes all cast functions for an ```Array``` construction.
     */
    class TypeCasts {
        public:
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
    
} // namespace runtime

#endif // RUNTIME_ARRAYRUNTIME_H