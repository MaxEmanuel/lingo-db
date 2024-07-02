#ifndef RUNTIME_ARRAYRUNTIME_H
#define RUNTIME_ARRAYRUNTIME_H
#include "runtime/helpers.h"

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
    
} // namespace runtime

#endif // RUNTIME_ARRAYRUNTIME_H