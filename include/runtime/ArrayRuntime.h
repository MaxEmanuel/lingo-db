#ifndef RUNTIME_ARRAYRUNTIME_H
#define RUNTIME_ARRAYRUNTIME_H
#include "runtime/helpers.h"

namespace runtime
{
    struct ArrayRuntime{
        /**
        * This function concatenates two arrays with same dimensions. It will be able to concatenate a low dimensional array (```array2```) into a high 
        * dimensional array (```array1``) but not otherwise.
        * @param array1     The array which should be extended with more elements
        * @param dim1       The dimension value of the first array
        * @param type1      The type of the elements in the first array
        * @param array2     The array which should be moved to the first array
        * @param dim2       The dimension value of the second array
        * @param type2      The type the elements in the second array
        * @returns          A modified array wrapped up as string in a VarLen32 object
        * @note             If one of the given arrays could not be created according to the given str, dim and type parameters or both arrays have different 
        *                   types, it will throw an ```std::runtime_error```. 
        */
        static runtime::VarLen32 concat(runtime::VarLen32 array1, uint64_t dim1, runtime::VarLen32 type1, runtime::VarLen32 array2, uint64_t dim2, runtime::VarLen32 type2);

        /**
         * This function allows to pick up some elements of a given array in a specific range.
         * @param str       The intial array
         * @param dim       The amount of dimension of the given array
         * @param type      The type of the elements from the given array
         * @param start     The start index or start value of the range specification
         * @param stop      The stop index or end value of the range specification
         * @returns         A modified array wrapped up as string in a VarLen32 object
         * @note            If the start, stop indices or subDim parameter are wrongly specified or the array could not be created according to the 
         *                  given str, dim and type parameters, it will throw an ```std::runtime_error```. 
         */
        static runtime::VarLen32 getRange(runtime::VarLen32 str, uint64_t dim, runtime::VarLen32 type, uint64_t start, uint64_t stop, uint64_t subDim);

        /**
         * This function allows to pick up a single element of a given array.
         * @param str       The intial array
         * @param dim       The amount of dimension of the given array
         * @param type      The type of the elements from the given array
         * @param index     The index of the element which should be returned
         * @returns         A modified array wrapped up as string in a VarLen32 object
         * @note            If the index is wrongly specified or the array could not be created according to the 
         *                  given str, dim and type parameters, it will throw an ```std::runtime_error```. 
         */
        static runtime::VarLen32 getEntry(runtime::VarLen32 str, uint64_t dim, runtime::VarLen32 type, uint64_t index);

        /**
         * This function returns a string of dimension values (e.g. '{{1,2}, {2,3}}' it will be '[1:2][1:2]').
         * @param str       The initial array
         * @param dim       The amount of dimension of the given array
         * @param type      The type of the elements from the given array
         * @returns         A VarLen32 object containing every dimension information
         * @note            If the array could not be created according to the given str, dim and type parameters, it will throw an 
         *                  ```std::runtime_error```.
         */
        static runtime::VarLen32 getDimensions(runtime::VarLen32 str, uint64_t dim, runtime::VarLen32 type);

        /**
         * This function returns the number of elements inside the array as ```VarLen32``` object.
         * @param str       The initial array
         * @param dim       The amount of dimension of the given array
         * @param type      The type of the elements from the given array
         * @returns         A VarLen32 object containing the number of elements in the array
         * @note            If the array could not be created according to the given str, dim and type parameters, it will throw an
         *                  ```std::runtime_error```.
         */
        static runtime::VarLen32 getCardinality(runtime::VarLen32 str, uint64_t dim, runtime::VarLen32 type);

        /**
         * This funtion allows the addition of two arrays.
         * @param str1       The array which should be modified
         * @param dim1       The dimension value of the first array
         * @param type1      The type of the elements in the first array
         * @param str2       The array which includes all elements for addition
         * @param dim2       The dimension value of the second array
         * @param type2      The type the elements in the second array
         * @returns          A modified array wrapped up as string in a ```VarLen32``` object
         * @note             It will throw an ```std::runtime_error```, if one of the given arrays could not be created according to the given 
         *                   ```str```, ```dim``` and ```type``` parameters
         * @note             It will throw an ```std::runtime_error```, if both arrays have different types
         * @note             It will throw an ```std::runtime_error```, if the dimension values of both arrays are not equal
         */
        static runtime::VarLen32 add(runtime::VarLen32 str1, uint64_t dim1, runtime::VarLen32 type1, runtime::VarLen32 str2, uint64_t dim2, runtime::VarLen32 type2);

        /**
         * This funtion allows the subtraction of two arrays.
         * @param str1       The array which should be modified
         * @param dim1       The dimension value of the first array
         * @param type1      The type of the elements in the first array
         * @param str2       The array which includes all elements for subtraction
         * @param dim2       The dimension value of the second array
         * @param type2      The type the elements in the second array
         * @returns          A modified array wrapped up as string in a ```VarLen32``` object
         * @note             It will throw an ```std::runtime_error```, if one of the given arrays could not be created according to the given 
         *                   ```str```, ```dim``` and ```type``` parameters
         * @note             It will throw an ```std::runtime_error```, if both arrays have different types
         * @note             It will throw an ```std::runtime_error```, if the dimension values of both arrays are not equal
         */
        static runtime::VarLen32 sub(runtime::VarLen32 str1, uint64_t dim1, runtime::VarLen32 type1, runtime::VarLen32 str2, uint64_t dim2, runtime::VarLen32 type2);

        static runtime::VarLen32 transpose(runtime::VarLen32 str, uint64_t dim, runtime::VarLen32 type);

        /**
         * This function casts an array (actual a single value which will be interpreted as array) to an integer with 32 bits.
         * @param str       The initial array
         * @returns         The value as int32_t
         * @note            If the array could not be converted to int, it will throw an ```std::runtime_error```.
         */
        static int32_t castToInt32(runtime::VarLen32 str);

        /**
         * This function casts an array (actual a single value which will be interpreted as array) to an integer with 64 bits.
         * @param str       The initial array
         * @returns         The value as int64_t
         * @note            If the array could not be converted to int, it will throw an ```std::runtime_error```.
         */
        static int64_t castToInt64(runtime::VarLen32 str);

        /**
         * This function casts an array (actual a single value which will be interpreted as array) to a float.
         * @param str       The initial array
         * @returns         The value as float
         * @note            If the array could not be converted to float, it will throw an ```std::runtime_error```.
         */
        static float castToFloat(runtime::VarLen32 str);

        /**
         * This function casts an array (actual a single value which will be interpreted as array) to a double.
         * @param str       The initial array
         * @returns         The value as double
         * @note            If the array could not be converted to double, it will throw an ```std::runtime_error```.
         */
        static double castToDouble(runtime::VarLen32 str);

        /**
         * This function casts an array (actual a single value which will be interpreted as array) to an array with other specifications.
         * E.g. the array should have another ```type```or another ```dimension``` value. 
         * @param str       The initial array
         * @param dim       The amount of dimension of the given array
         * @param type      The type of the elements from the given array
         * @returns         The value as new array
         * @note            If the array could not be converted to other array, it will throw an ```std::runtime_error```.
         */
        static runtime::VarLen32 castToArray(runtime::VarLen32 str, uint64_t dim, runtime::VarLen32 type);
    };
    
} // namespace runtime

#endif // RUNTIME_ARRAYRUNTIME_H;