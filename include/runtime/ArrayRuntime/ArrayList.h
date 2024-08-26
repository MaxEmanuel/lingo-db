#include <algorithm>
#include <functional>
#include <stdexcept>
#include <vector>
#include <string>
#include "runtime/ArrayRuntime/ArrayElement.h"
#include "runtime/ArrayRuntime/ArrayArith.h"

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
        // Number of elements
        size_t size;
        // The dimension of the object in the array
        uint64_t dimension;
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
        ArrayList(std::string array, uint64_t dimension, TypeCast typeCast, StringCast stringCast) : dimension(dimension), isNull(false) {
            this->elements = std::vector<ArrayElem<R>>();
            this->container = std::vector<ArrayList<R>>();
            if (dimension == 1) {
                this->setElements(array, typeCast, stringCast);
                this->size = this->elements.size();
            } else {
                this->setContainer(array, typeCast, stringCast);
                this->size = this->container.size();
            }
        }

        /**
         * This constructor creates an empty instance of ```ArrayList``` which represents a ```null``` value.
         */
        ArrayList() : elements(std::vector<ArrayElem<R>>()), container(std::vector<ArrayList<R>>()), size(0), dimension(0), isNull(true) {}

        /*################################################################################################################################################
                                                                    ARRAY MANIPULATION
        #################################################################################################################################################*/

        /**
         * This method converts the content of a ```ArrayList``` object to a string 
         * @note            If the object represents a ```null``` value it will return "null, "
         */
        std::string toString() {
            if (this->isNull){
                return "null, ";
            } 
            std::string result = "{";
            switch (this->dimension) {
            // If container contains single elements    
            case 1:
                for (auto& element : this->elements) {
                    result += element.toString();
                }
                break;
            // If container contains lists of elements    
            default:
                for (auto& element : this->container) {
                    result += element.toString();
                }
                break;
            }
            // Delete the last characters which are ', '
            result = result.substr(0, result.size() - 2);
            result += "}, ";
            return result;
        }

        /**
         * This method joins two arrays together. It is also possible that the given ```array``` has a lower dimension than ```this```.
         * In this case it will add the complete ```array``` as last entry to the next higher dimension (e.g. if ```array``` has an dimension
         * of 1 it will be added to the dimension 2 of ```this```).
         * @param array         A reference to the ```ArrayList``` object which should be connected to ```this```
         * @note                If the dimension value of the parameter ```array``` is larger than the dimension value of ```this```,
         *                      it will throw an ```std::runtime_error```.
         */
        void concat(ArrayList<R>& array) {
            // Compare both dimension values
            switch (this->dimension - array.getDimension()) {
            // If both dimensions are equal, extract childs from parameter and push it to this
            case 0:
                if (this->dimension == 1) {
                    for (auto& element : array.getElements()) {
                        this->elements.push_back(element);
                    }
                } else {
                    for (auto& element: array.getContainer()) {
                        this->container.push_back(element);
                    }
                }
                break;
            // If the difference is +1, then push complete parameter to this
            case 1:
                this->container.push_back(array);
                break;
            // If the difference is +2,..., infinte, try to push element to last child, otherwise throw error
            default:
                if (this->dimension - array.getDimension() > 1) {
                    this->container[this->size - 1].concat(array);
                } else {
                    throw std::runtime_error("Left array dimension -" + std::to_string(this->dimension) + "- < -" + std::to_string(array.getDimension()) + "- as dimension of the right array.");
                }
            }
        }

        /**
         * This method returns a single entry from the array and returns it as string.
         * @param index         The index of the element which should be returned (startint at 0)
         * @returns             The element as string
         * @note                If the index does not map to an existing element it will throw an ```std::runtime_error```
         */
        std::string getEntry(uint64_t index) {
            // If array element is null
            if (this->isNull) {
                return "null";
            }
            // If given index is out of bounds
            if (this->size <= index) {
                throw std::runtime_error("The desired element on " + std::to_string(index) + " does not exist");
            }
            std::string result;
            switch (this->dimension) {
            // If container contains single elements
            case 1:
                result = this->elements[index].toString();
                break;
            // If container contains lists of elements
            default:
                result = this->container[index].toString();
                break;
            }
            return result.substr(0, result.size() - 2);
        }

        /**
         * This method changes the structure of the elements. All elements of a particular dimension which are between the indices
         * ```start``` and  ```stop``` will remain. All other elements will be deleted.
         * @param start         The start index of the interval
         * @param stop          The end index of the interval
         * @param subDim        The dimension which should be adjusted
         * @note                A ```std::runtime_error``` will be thrown if one of the parameters is larger then expected 
         *                      (e.g. ```start = 3``` but array has only 2 elements)
         * @note                If the object represents a ```null``` value, it will return without any changes
         */
        void slice(uint64_t start, uint64_t stop, uint64_t subDim) {
            // If there is a null value, nothing to slice
            if (this->isNull) {
                return;
            }
            // Proof if given subDim is out of bounds
            if (subDim > this->dimension) {
                throw std::runtime_error("This array does not have a " + std::to_string(subDim) + "th dimension");
            // Proof if the current dimension needs to be adjusted
            } else if (subDim == this->dimension) {
                // Proof if start or stop index is out of bounds
                if (this->size <= start) {
                    throw std::runtime_error("This array does not have any elements at " + std::to_string(start) + " in dimension " + std::to_string(subDim));
                }
                if (this->size <= stop) {
                    throw std::runtime_error("This array does not have any elements at " + std::to_string(stop) + " in dimension " + std::to_string(subDim));
                }
                size_t index = 0;
                switch (this->dimension) {
                case 1:
                    // Iterate over single elements and delete those that does not appear in the interval
                    for (auto it = this->elements.begin(); it != this->elements.end(); it++) {
                        if (index < start || index > stop) {
                            this->elements.erase(it);
                            it--;
                        }
                        index++;
                    }
                    break;
                default:
                    // Iterate over complete container and delete those that does not appear in the interval
                    for (auto it = this->container.begin(); it != this->container.end(); it++) {
                        if (index < start || index > stop) {
                            this->container.erase(it);
                            it--;
                        }
                        index++;
                    }
                    break;
                }
            } else {
                // Call the function of the children, because lower dimension is given
                for (auto& element : this->container) {
                    element.slice(start, stop, subDim);
                }
            }
        }

        /**
         * This method returns for each dimension the accessible range as string (e.g. '[1:2][1:3]').
         * @return              A string which contains the dimensionality of the array
         * @note                Currently this method only returns the dimensionality from the first elements that fit. This means
         *                      that not all elements may fit with this schema. Currently it will not be checked that each element inside
         *                      the array will have the same structure (e.g. '{{1,2},{1,2,3}}' is a valid array and will return '[1:2][1:2]')
         * @note                If a container does not contain any elements and is not ``ǹull```, then this function returns '[0:0]'
         * @note                If the container represents a ``ǹull``` value, it will return an empty string
         */
        std::string getDimensionRange() {
            // Proof if null value
            if (this->isNull) {
                return "";
            }
            // Proof if empty list
            if (this->size == 0) {
                 return "[0:0]";
            }
            // Proof if container consists of a list of single elements
            if (this->dimension == 1) {
                return "[1:" + std::to_string(this->size) + "]";
            }
            std::string result = "[1:" + std::to_string(this->size) + "]";
            std::string subResult = "";
            // Find best fitting element
            for (auto& element : this->container) {
                // Ensure that null values and empty lists will not be present in final result
                if (subResult == "" || subResult.find("[0:0]") != std::string::npos || static_cast<uint64_t>(std::ranges::count(subResult, '[')) != this->dimension - 1) {
                    subResult = element.getDimensionRange();
                } else {
                    return result + subResult;
                }
            }
            return result + "[0:0]";
        }

        /**
         * This method returns the number of elements in the array.
         * @return              The number of elements in the array
         */
        uint64_t getNumberElements() {
            // Proof if this represents a null value
            if (this->isNull) {
                return 1;
            }
            if (this->dimension == 1) {
                return this->elements.size();
            } else {
                uint64_t result = 0;
                for (auto& element : container) {
                    result += element.getNumberElements();
                }
                return result;
            }
        }

        /*################################################################################################################################################
                                                                        MATH FUNCTIONS (ML)
        #################################################################################################################################################*/

        /**
         * This method implements the element-wise arithmetic operations for an array (Currently addition, subtraction and multiplication).
         * @param other         A reference to the ```ArrayList``` object which elements represents the right side of the equation
         * @param op            The operation which should be done
         * @note                If the dimension values are not equal of both arrays, it will throw an ```std::runtime_error```
         * @note                If the given ```op``` does not exist (implemented), it will throw an ```std::runtime_error```
         * @note                If one array has less elements than the other array, all elements which exceeds the boundary of
         *                      the smallest array will be ignored
         * @note                If the current container or ```other```  represents a ```null``` value, this container will be changed to 
         *                      a ```null``` value.
         */
        void elementWiseArith(ArrayList<R>& other, ArrayArithOperator op) {
            // Proof if one of the containers is a null value
            if (this->isNull || other.getIsNull()){
                this->isNull = true;
                return;
            }
            // Proof if both arrays have the same dimension
            if (this->dimension != other.getDimension()){
                throw std::runtime_error("Both arrays should have the same dimension");
            }
            // Use end index of the container which has less elements
            auto boundary = this->size > other.getSize() ? other.getSize() : this->size;
            // Call corresponding operator for every single array element
            if (this->dimension == 1) {
                for (size_t index = 0; index < boundary; index++) {
                    auto otherElement = other.getElements()[index];
                    switch (op) {
                    case ArrayArithOperator::addition:
                        this->elements[index].add(otherElement);
                        break;
                    case ArrayArithOperator::subtraction:
                        this->elements[index].sub(otherElement);
                        break;
                    case ArrayArithOperator::multiplication:
                        this->elements[index].mult(otherElement);
                        break;
                    default:
                        throw std::runtime_error("The entered operator for arrays is currently not supported");
                    }
                }
            // Call this function for all children with their corresponding counterpart
            } else {
                for (size_t index = 0; index < boundary; index++) {
                    this->container[index].elementWiseArith(other.getContainer()[index], op);
                }
            }
        }

        /**
         * This method allows scalar multiplication with any primitive numeric type (currently int32_t, int64_t, float and double).
         * @param value         The value which represents the scalar.
         * @note                If the type of the scalar does not match with the specification it will throw an ```std::runtime_error```
         */
        template <typename T>
        void scalarMult(T value) {
            if (!std::is_same<int32_t, T>::value || !std::is_same<int64_t, T>::value || !std::is_same<float, T>::value || !std::is_same<double, T>::value) {
                std::runtime_error("Scalar-multiplication is only allowed with primitive numeric types");
            }
            if (this->isNull) {
                return;
            }
            switch (this->dimension) {
            case 1:
                for (auto& element : this->elements) {
                    element.mult(value);
                }
                break;
            default:
                for (auto& element : this->container) {
                    element.scalarMult(value);
                }
                break;
            }
        }

        /**
         * This method computes the result of a matrix mulitplication with any primitive numeric type (currently int32_t, int64_t, float and double).
         * @param other             The array which should be multiplied upon this array.
         * @return                  The resulting array as string
         * @note                    Currently this method is only supported for arrays with at most 2 dimensions. If one of the given arrays have more
         *                          dimensions then an ```std::runtime_error``` will be thrown.
         * @note                    This operation can only be used if both arrays have a uniform number of elements (e.g. '{{1,2},{1,2}}' or '{1,2,3}', 
         *                          but not '{{1,2}, {1,2,3}}') - otherwise an ```std::runtime_error``` will be thrown.
         * @note                    Both arrays should not contain any ```null``` values, otherwise an ```std::runtime_error``` will be thrown.
         * @note                    Number of columns of the first array must equal the number of rows of the second array (according to definition of matrix
         *                          multiplication), otherwise an ```std::runtime_error``` will be thrown.
         */
        std::string matrixMult(ArrayList<R>& other) {
            std::vector<ArrayList<R>> result;
            if (this->dimension > 2 || other.getDimension() > 2) {
                throw std::runtime_error("Matrix multiplication is currently only supported with maximal 2 dimensions");
            }
            if (this->isNull || other.getIsNull()) {
                throw std::runtime_error("Null values are not allowed here");
            }
            // Numberof columns of matrix A
            size_t numberACol = this->dimension == 1 ? this->dimension : this->size;
            // Number of rows of matrix A
            size_t numberARows = this->dimension == 1 ? this->size : this->getMaxChildSize();
            // Number of columns of matrix B
            size_t numberBCol = other.getDimension() == 1 ? other.getDimension() : other.getSize();
            // Iterate over all columns of matrix B
            for (size_t bIndex = 0; bIndex < numberBCol; bIndex++) {
                ArrayList<R> newColumn;
                // Get std::vector<ArrayElem<R>> or std::vector<ArrayList<R>> representing one column of matrix B
                std::vector<ArrayElem<R>> bColumn = numberBCol == 1 ? other.getElements() : other.getContainer()[bIndex].getElements();
                if (numberACol != bColumn.size()) {
                    throw std::runtime_error("Matrix multiplication with, " + std::to_string(this->size) + ":" + std::to_string(bColumn.size()) + ", unequal dimensions not possible");
                }
                // Iterate over all rows of matrix A
                for (size_t aRowIndex = 0; aRowIndex < numberARows; aRowIndex++) {
                    ArrayElem<R> value(0);
                    // Iterate over all columns of matrix A
                    for (size_t aColumnIndex = 0; aColumnIndex < numberACol; aColumnIndex++) {
                        // Get std::vector<ArrayElem<R>> or std::vector<ArrayList<R>> representing one column of matrix A
                        std::vector<ArrayElem<R>> aColumn = this->dimension == 1 ? this->elements : this->container[aColumnIndex].getElements();
                        if (aColumn.size() <= aRowIndex) {
                            throw std::runtime_error("First matrix has a uneven number of elements and is therefore not suited for matrix multiplication");
                        }
                        // Get single ArrayElems and multiply them together
                        auto aValue = aColumn[aRowIndex];
                        auto bValue = bColumn[aColumnIndex];
                        if (aValue.getIsNull() || bValue.getIsNull()) {
                            throw std::runtime_error("Null values are not allowed here");
                        }
                        value.add(aValue.getValue() * bValue.getValue());
                    }
                    // Add new value to new container representing a single column
                    newColumn.addElement(value);
                }
                // Add new column to new container representing the complete matrix
                result.push_back(newColumn);
            }
            // Adjust the current container and return the result as string by calling toString method
            if (numberBCol == 1) {
                this->dimension = 1;
                this->elements = result[0].getElements();
                if (this->elements.size() == 1) {
                    std::string result = this->elements[0].toString();
                    return result.substr(0, result.size() - 2);
                }
            } else {
                this->dimension = 2;
                this->container = result;
            }
            std::string array = this->toString();
            return array.substr(0, array.size() - 2);
        }

        /**
         * This method implements a transpose mechanism to an ```ArrayList``` object. This means the current ```dimension```
         * of this object will be switched with the below ```dimension```. This function will therefore rearrange all affected
         * elements. E.g. '{{1,2,3},{4,5,6}}' will be changed to '{{1,4},{2,5},{3,6}}', because this object has a 2x3 structure
         * and will be changed to a 3x2 structure.
         * @note
         */
        void transpose(){
            std::vector<ArrayList<R>> result;
            size_t rows = this->dimension == 1 ? this->size : this->getMaxChildSize();
            // Iterate over 0 to new upper dimension (goal: merge every entry with a common index together)
            for (size_t row = 0; row < rows; row++) {
                ArrayList<R> newElement;
                // Special case: array with 1 colunmn -> array with n columns and 1 row
                if (this->dimension == 1) {
                    auto& element = this->elements[row];
                    if (element.getIsNull()) {
                        ArrayElem<R> value;
                        newElement.addElement(value);
                    } else {
                        newElement.addElement(element);
                    }
                // Case: array with n columns and k rows
                } else {
                    // Iterate over all children to be able to access their list elements
                    for (auto& element : this->container) {
                        if (this->dimension == 2) {
                            // If this element is a null value, add also a null value to the new ArrayList
                            if (element.getIsNull()){
                                ArrayElem<R> value;
                                newElement.addElement(value);
                            // If current element has an entry on index row (otherwise do nothing)
                            } else if (element.getSize() > row) {
                                // Add this element as an entry to the new ArrayList
                                newElement.addElement(element.getElements()[row]);
                            }
                        } else if (this->dimension > 2) {
                            // If this element is a null value, add also a null value to the new ArrayList
                            if (element.getIsNull()){
                                ArrayList<R> value;
                                newElement.addContainer(value, this->dimension - 1);
                            // If current element has an entry on index row (otherwise do nothing)
                            } else if (element.getSize() > row) {
                                // Add this element as an entry to the new ArrayList
                                newElement.addContainer(element.getContainer()[row], this->dimension - 1);
                            }
                        }
                    }
                }
                result.push_back(newElement);
            }
            // Adjust dimension value especially for special case like [5:1] -> [1:5] or [1:5] -> [5:1]
            if (this->dimension == 2 && rows == 1) {
                this->dimension = 1;
                this->elements = result[0].getElements();
            } else if (this->dimension == 1 && rows > 1) {
                this->dimension = 2;
                this->container = result;
            } else {
                this->container = result;
            }
        }

        /*################################################################################################################################################
                                                                        GETTER / SETTER
        #################################################################################################################################################*/

        /**
         * This method returns the dimension value of the current ```ArrayList``` object.
         * @returns             An dimension value
         */
        uint64_t getDimension(){
            return this->dimension;
        }

        /**
         * This method returns a ```bool``` which signals if this ```ArrayList``` represents a ```null``` value.
         * @returns             A ```bool``` if the current object represents a ```null``` value.
         */
        bool getIsNull(){
            return this->isNull;
        }

        /**
         * This method returns the number of elements stored in this container
         * @returns             The size of the corresponding stored list
         */
        size_t getSize(){
            return this->size;
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

        /**
         * This method allows to add a new ```ArrayElem``` object, to the current list. If the current container
         * represents a ```null``` value, it will be reversed.
         * @param element       A reference to the ```ArrayElem``` object which should be added to the container list.
         */
        void addElement(ArrayElem<R>& element){
            // Init attributes if container was a null value
            if (this->isNull) {
                this->isNull = false;
                this->dimension = 1;
                this->size = 0;
            }
            this->elements.push_back(element);
            this->size++;
        }

        /**
         * This method allows to add a new ```ArrayList``` object, to the current list. If the current container
         * represents a ```null``` value, it will be reversed.
         * @param element       A reference to the ```ArrayList``` object which should be added to the container list.
         * @param dimension     A value which represents the ```dimension``` of the current container (is needed if current
         *                      object represents a ```null``` value)
         */
        void addContainer(ArrayList<R>& container, uint64_t dimension) {
            if (this->isNull) {
                this->isNull = false;
                this->dimension = dimension;
                this->size = 0;
            }
            if ((container.getDimension() + 1 != this->dimension) && !container.getIsNull()) {
                std::runtime_error("A list of array element could not be added according to wrong dimension specification: " + std::to_string(this->dimension) + " != " + std::to_string(container.getDimension() + 1));
            }
            this->container.push_back(container);
            this->size++;
        }

        /*################################################################################################################################################
                                                                        PRIVATE METHODS
        #################################################################################################################################################*/

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
                    throw std::runtime_error("The entered array instance has a smaller dimension than specified. Please ensure that your instance should have a dimension of: " + std::to_string(this->dimension)); 
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
         */
        void setElements(std::string array, TypeCast typeCast, StringCast stringCast) {
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
                        // If empty, leave it empty
                        if (index - startIndex == 0){
                            continue;
                        }
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

        /**
         * This method returns a value which represents the largest list size of all children in the ```container```
         * or ```elements``` attribute (depends on the dimension of the current object).
         * @return          A value representing the number of elements in the next below dimension
         */
        size_t getMaxChildSize() {
            // If the container contains only ArrayElems -> return 1
            if (this->dimension == 1) {
                return 1;
            }
            // Find the maximum list size of all children
            size_t result = 1;
            for (auto& element : this->container) {
                if (result < element.getSize()) {
                    result = element.getSize();
                }
            }
            return result;
        }
    };
} // namespace runtime
