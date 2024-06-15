#ifndef RUNTIME_ARRAYRUNTIME_H
#define RUNTIME_ARRAYRUNTIME_H
#include "runtime/helpers.h"
#include <vector>
#include <memory>

namespace runtime
{
    struct ArrayRuntime{
        /**
         * This static function allows to cast an string containing an array to a std::vector with one dimension with 
         * elements of type int32_t. To be able of distinguishing between existing values and NULL values the resulting std::vector
         * contains pointer to single elements. BE AWARE that NULL values will be represented as nullptr. If the type does not match, 
         * it will throw an runtime_error.
         * @param array The array as string being a VarLen32 object
         * @param container The std::vector container to which the elements should be added 
         */
        static void toVector(VarLen32 array, std::vector<std::unique_ptr<int32_t>>& container);

        static void toVector(VarLen32 array, std::vector<std::unique_ptr<int64_t>>& container);

        static void toVector(VarLen32 array, std::vector<std::unique_ptr<float>>& container);

        static void toVector(VarLen32 array, std::vector<std::unique_ptr<double>>& container);

        static void toVector(VarLen32 array, std::vector<std::unique_ptr<std::string>>& container);

        /**
         * This static function allows to cast a string, containing a list of arrays, to a std::vector with two dimensions with 
         * elements of type int32_t. To be able of distinguishing between existing values and NULL values the resulting std::vector
         * contains pointer to other std::vector. BE AWARE that NULL values will be represented as nullptr. If the type does not 
         * match or the dimensions are too low, it will throw an runtime_error.
         * @param array The array as string being a VarLen32 object
         * @param container The std::vector container to which the elements should be added 
         */
        static void toMatrix(VarLen32 array, std::vector<std::unique_ptr<std::vector<std::unique_ptr<int32_t>>>>& container);

        static void toMatrix(VarLen32 array, std::vector<std::unique_ptr<std::vector<std::unique_ptr<int64_t>>>>& container);

        static void toMatrix(VarLen32 array, std::vector<std::unique_ptr<std::vector<std::unique_ptr<float>>>>& container);

        static void toMatrix(VarLen32 array, std::vector<std::unique_ptr<std::vector<std::unique_ptr<double>>>>& container);

        static void toMatrix(VarLen32 array, std::vector<std::unique_ptr<std::vector<std::unique_ptr<std::string>>>>& container);

        /**
         * This static function converts a std::vector with single int32_t elements to a VarLen32 object.
         * @param container The std::vector which contains the elements
         */
        static runtime::VarLen32 fromVector(std::vector<std::unique_ptr<int32_t>>& container);

        static runtime::VarLen32 fromVector(std::vector<std::unique_ptr<int64_t>>& container);

        static runtime::VarLen32 fromVector(std::vector<std::unique_ptr<float>>& container);

        static runtime::VarLen32 fromVector(std::vector<std::unique_ptr<double>>& container);

        static runtime::VarLen32 fromVector(std::vector<std::unique_ptr<std::string>>& container);

        /**
         * This static function converts a list of std::vectors with single int32_t elements to a VarLen32 object.
         * @param container The std::vector which contains the list of vectors
         */
        static runtime::VarLen32 fromMatrix(std::vector<std::unique_ptr<std::vector<std::unique_ptr<int32_t>>>>& container);

        static runtime::VarLen32 fromMatrix(std::vector<std::unique_ptr<std::vector<std::unique_ptr<int64_t>>>>& container);

        static runtime::VarLen32 fromMatrix(std::vector<std::unique_ptr<std::vector<std::unique_ptr<float>>>>& container);

        static runtime::VarLen32 fromMatrix(std::vector<std::unique_ptr<std::vector<std::unique_ptr<double>>>>& container);

        static runtime::VarLen32 fromMatrix(std::vector<std::unique_ptr<std::vector<std::unique_ptr<std::string>>>>& container);
    };
} // namespace runtime


#endif // RUNTIME_ARRAYRUNTIME_H