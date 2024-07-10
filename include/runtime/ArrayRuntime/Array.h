#include <string>
#include <functional>
#include <stdexcept>
#include <vector>
#include "runtime/helpers.h"
#include "runtime/ArrayRuntime/ArrayList.h"

namespace runtime
{
    /**
       This class represents an array structure with abitrary dimensions. The type ```T``` defines which type the corresponding elements should have.
     */
    template<typename T>
    class Array {

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
            return this->castToVarLen(result);
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
         * This method returns a single entry from the array and returns it as ```VarLen32``` object.
         * @param index         The index of the element which should be returned (starting at 1)
         * @returns             The element as ```VarLen32``` object
         * @note                If the index does not map to an existing element it will throw an ```std::runtime_error```
         */
        runtime::VarLen32 getEntry(int32_t index) {
            std::string result = this->array.getEntry(index - 1);
            return this->castToVarLen(result);
        }

        /**
         * This method changes the structure of the elements. All elements of a particular dimension which are between the indices
         * ```start``` and  ```stop``` will remain. All other elements will be deleted.
         * @param start         The start index of the interval
         * @param stop          The end index of the interval
         * @param subDim        The dimension which should be adjusted
         * @note                Be aware that the first element will be on index 1 not 0.
         * @note                A ```std::runtime_error``` will be thrown if ```start < 1```, ```stop < 1```, ```subDim < 1``` or
         *                      all these parameters are larger then expected (e.g. ```start = 3``` but array has only 2 elements)
         */
        void slice(int32_t start, int32_t stop, int32_t subDim) {
            // reduce start and stop value, because according to specification it should start with 1 and not 0.
            start--; 
            stop--;
            if (start < 0){
                throw std::runtime_error("Invalid index value: " + std::to_string(start));
            }
            if (stop < 0) {
                throw std::runtime_error("Invalid index value: " + std::to_string(stop));
            }
            if (subDim < 1) {
                throw std::runtime_error("Invalid dimension value: " + std::to_string(subDim));
            }
            this->array.slice(start, stop, subDim);
        }

        /**
         * This method returns for each dimension the accessible range as ```VarLen32``` object (e.g. '[1:2][1:3]').
         * @return              A ```VarLen32``` object which contains the string about the dimensionality of the array
         * @note                Currently this method only returns the dimensionality from the first elements that fit. This means
         *                      that not all elements fit with this schema. Currently it will not be checked that each element inside
         *                      the array will have the same structure (e.g. '{{1,2},{1,2,3}}' is a valid array and will return '[1:2][1:2]')
         */
        runtime::VarLen32 getDimensionRange() {
            std::string result = this->array.getDimensionRange();
            return this->castToVarLen(result);
        }

        /**
         * This method returns the number of elements in the array.
         * @return              The number of elements in the array as string in a ```VarLen32``` object
         * @note                ```null``` values will be counted if they replace a single array element (e.g. '{1,2,3,null}' will return 4, 
         *                      but '{{1,2}, null}' will return 2).
         */
        runtime::VarLen32 getCardinality() {
            int32_t result = this->array.getNumberElements();
            return this->castToVarLen(std::to_string(result));
        }

        /**
         * This method implements the element-wise addition for an array.
         * @param toAdd         A reference to the ```Array``` object which elements should be used for addition
         * @note                If the dimension values are not equal of both arrays, it will throw an ```std::runtime_error```
         * @note                If one array has less elements than the other array, all elements which exceeds the boundary of
         *                      the smallest array will be ignored
         */
        void add(Array<T>& toAdd) {
            this->array.add(toAdd.getArray());
        }

        /**
         * This method implements the element-wise subtraction for an array.
         * @param toSub         A reference to the ```Array``` object which elements should be used for subtraction
         * @note                If the dimension values are not equal of both arrays, it will throw an ```std::runtime_error```
         * @note                If one array has less elements than the other array, all elements which exceeds the boundary of
         *                      the smallest array will be ignored
         */
        void sub(Array<T>& toSub) {
            this->array.sub(toSub.getArray());
        }

        void transpose() {
            this->array.transpose();
        }

        /**
         * This method returns a reference to the ```ArrayList``` attribute from this class
         * @return              A reference to the ```ArrayList``` value
         */
        ArrayList<T>& getArray(){
            return this->array;
        }

        private:
        /**
         * This function casts a string to a ```VarLen``` object.
         * @param data          The string which should be converted
         * @returns             A ```VarLen``` object which contains the given string
         */
        runtime::VarLen32 castToVarLen (std::string data) {
            // These are necessary steps to create a VarLen32 object
            char* result = new char[data.length()];           
            memcpy(result, data.data(), data.length());     
            return runtime::VarLen32((uint8_t*) result, data.length());
        }

    };
} // namespace runtime