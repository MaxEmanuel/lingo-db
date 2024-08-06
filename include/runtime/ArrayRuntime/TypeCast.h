#include <string>
#include <algorithm>

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
         * characters from the string: ```' '```,```'``` and ```"```. 
         */
        static std::string stringToString(std::string value) {
            value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
            value.erase(std::remove(value.begin(), value.end(), '\''), value.end());
            value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
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
