#include <functional>
#include <stdexcept>

namespace runtime {
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
}