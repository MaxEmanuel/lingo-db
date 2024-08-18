#include <string>
#include <algorithm>
#include <type_traits>

namespace runtime
{
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
         * This static method converts a std::string into a std::string. It will remove the following 
         * characters from the string: ```'``` and ```"```. 
         */
        static std::string stringToString(std::string value) {
            value.erase(std::remove(value.begin(), value.end(), '\''), value.end());
            value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
            return value;
        };

        /**
         * This static method converts a primitive numeric value into a std::string
         */
        template<typename R>
        static std::string numericToString(R value) {
            std::string result = std::to_string(value);
            if (std::is_same<R, float>::value || std::is_same<R, double>::value) {
                // Remove trailing zeros
                result.erase(result.find_last_not_of('0') + 1, std::string::npos);
    
                // Remove the decimal point if it's the last character
                if (result.back() == '.') {
                    result.pop_back();
                }
            }
            return result;
        };

        /**
         * This static method converts a primitive numeric value into a ```runtime::VarLen32``` defined as an array, e.g. '{1}'
         */
        template<typename R>
        static runtime::VarLen32 numericToArrayVarLen(R value, uint64_t dim) {
            std::string result = std::to_string(value);
            result = std::string(dim, '{') + result + std::string(dim, '}');
            char* array = new char[result.length()];           
            memcpy(array, result.data(), result.length());     
            return runtime::VarLen32((uint8_t*) array, result.length());
        };
    };
} // namespace runtime
