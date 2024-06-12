#ifndef RUNTIME_ARRAYRUNTIME_H
#define RUNTIME_ARRAYRUNTIME_H
#include "runtime/helpers.h"
#include <vector>
#include <memory>

namespace runtime
{
    struct ArrayRuntime{
        /**
         * This static function allows to cast an string (containing an array) to a std::vector with one dimension whereby 
         * all elements should be of type int32_t. If the type does not match, it will throw an runtime_error.
         * @param array The array as string being a VarLen32 object
         * @param container The std::vector container to which the elements should be added 
         */
        static void toVector(VarLen32 array, std::vector<std::unique_ptr<int32_t>>& container);

        /**
         * This static function allows to cast an string (containing an array) to a std::vector with one dimension whereby 
         * all elements should be of type int64_t. If the type does not match, it will throw an runtime_error.
         * @param array The array as string being a VarLen32 object
         * @param container The std::vector container to which the elements should be added 
         */
        static void toVector(VarLen32 array, std::vector<std::unique_ptr<int64_t>>& container);

        /**
         * This static function allows to cast an string (containing an array) to a std::vector with one dimension whereby 
         * all elements should be of type float. If the type does not match, it will throw an runtime_error.
         * @param array The array as string being a VarLen32 object
         * @param container The std::vector container to which the elements should be added 
         */
        static void toVector(VarLen32 array, std::vector<std::unique_ptr<float>>& container);

        /**
         * This static function allows to cast an string (containing an array) to a std::vector with one dimension whereby 
         * all elements should be of type double. If the type does not match, it will throw an runtime_error.
         * @param array The array as string being a VarLen32 object
         * @param container The std::vector container to which the elements should be added 
         */
        static void toVector(VarLen32 array, std::vector<std::unique_ptr<double>>& container);

        /**
         * This static function allows to cast an string (containing an array) to a std::vector with one dimension whereby 
         * all elements should be of type string. If the type does not match, it will throw an runtime_error.
         * @param array The array as string being a VarLen32 object
         * @param container The std::vector container to which the elements should be added 
         */
        static void toVector(VarLen32 array, std::vector<std::unique_ptr<std::string>>& container);

        /**
         * This static function allows to cast an string (containing a array with arrays) to a std::vector with two dimension whereby 
         * all elements should be of type int32_t. If the type does not match or the dimensions are too low, it will throw an runtime_error.
         * @param array The array as string being a VarLen32 object
         * @param container The std::vector container to which the elements should be added 
         */
        static void toMatrix(VarLen32 array, std::vector<std::unique_ptr<std::vector<std::unique_ptr<int32_t>>>>& container);

        /**
         * This static function allows to cast an string (containing a array with arrays) to a std::vector with two dimension whereby 
         * all elements should be of type int64_t. If the type does not match or the dimensions are too low, it will throw an runtime_error.
         * @param array The array as string being a VarLen32 object
         * @param container The std::vector container to which the elements should be added 
         */
        static void toMatrix(VarLen32 array, std::vector<std::unique_ptr<std::vector<std::unique_ptr<int64_t>>>>& container);

        /**
         * This static function allows to cast an string (containing a array with arrays) to a std::vector with two dimension whereby 
         * all elements should be of type float. If the type does not match or the dimensions are too low, it will throw an runtime_error.
         * @param array The array as string being a VarLen32 object
         * @param container The std::vector container to which the elements should be added 
         */
        static void toMatrix(VarLen32 array, std::vector<std::unique_ptr<std::vector<std::unique_ptr<float>>>>& container);

        /**
         * This static function allows to cast an string (containing a array with arrays) to a std::vector with two dimension whereby 
         * all elements should be of type double. If the type does not match or the dimensions are too low, it will throw an runtime_error.
         * @param array The array as string being a VarLen32 object
         * @param container The std::vector container to which the elements should be added 
         */
        static void toMatrix(VarLen32 array, std::vector<std::unique_ptr<std::vector<std::unique_ptr<double>>>>& container);

        /**
         * This static function allows to cast an string (containing a array with arrays) to a std::vector with two dimension whereby 
         * all elements should be of type string. If the type does not match or the dimensions are too low, it will throw an runtime_error.
         * @param array The array as string being a VarLen32 object
         * @param container The std::vector container to which the elements should be added 
         */
        static void toMatrix(VarLen32 array, std::vector<std::vector<std::string>>& container);
    };
} // namespace runtime


#endif // RUNTIME_ARRAYRUNTIME_H