#include <algorithm>
#include <functional>
#include <stdexcept>
#include <vector>
#include <string>
#include "runtime/ArrayRuntime/ArrayElement.h"

namespace runtime
{
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
        void concat(ArrayList<R>& array) {
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
         * This method returns a single entry from the array and returns it as string.
         * @param index         The index of the element which should be returned
         * @returns             The element as string
         * @note                If the index does not map to an existing element it will throw an ```std::runtime_error```
         */
        std::string getEntry(int32_t index) {
            if (this->dimension == 1) {
                if ((int32_t) this->elements.size() <= index || index < 0){
                    throw std::runtime_error("The desired element does not exist");
                }
                std::string result = this->elements[index].toString();
                return result.substr(0, result.size() - 1);
            } else {
                if ((int32_t) this->container.size() <= index || index < 0){
                    throw std::runtime_error("The desired element does not exist");
                }
                std::string result = this->container[index].toString();
                return result.substr(0, result.size() - 1);
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
} // namespace runtime
