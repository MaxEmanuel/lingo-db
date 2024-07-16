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
                std::string result = this->castToString(this->value);
                // If type is string add "" to the value, to mark the element as string
                if (result.find('"') == std::string::npos && std::is_same<R, std::string>::value) {
                    result = '"' + result + '"';
                }
                return result + ", ";
            }
        }

        /**
         * This method returns a ```bool``` which signals if this ```ArrayElem``` 
         * represents a ```null``` value.
         */
        bool getIsNull() {
            return this->isNull;
        }

        /**
         * This method returns the value of the ```ArrayElem```.
         * @note        If the ```ArrayElem``` object represents a ```null``` value, this value can be a random value
         */
        R getValue() {
            return this->value;
        }

        /**
         * This method adds a value to the stored value of this ```ArrayElem```.
         * @param value         The value which should be added
         */
        void add(ArrayElem<R>& value) {
            this->value += value.getValue();
        }

        /**
         * This method subtracts a value to the stored value of this ```ArrayElem```.
         * @param value         The value which should be subtracted
         */
        void sub(ArrayElem<R>& value) {
            this->value -= value.getValue();
        }

        /**
         * This method multiplies a value to the stored value of this ```ArrayElem```.
         * @param value         The value which should be used for multiplication
         */
        void mul(ArrayElem<R>& value) {
            this->value *= value.getValue();
        }
    };
}