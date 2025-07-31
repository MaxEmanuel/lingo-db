#ifndef RUNTIME_ARRAY_H
#define RUNTIME_ARRAY_H

#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <tuple>
#include "runtime/ArrayArithmetic.h"
#include "runtime/helpers.h"

namespace runtime {

/**
 * This class represents an array structure in LingoDB.
 * 
 * @example - '{{{1,2,3},{1,null,3}},{{1,2,3},{1,2,3}}}'
 * type = 0
 * dimensions = 3
 * size = 11
 * indices = 1,1,1
 * dimensionWidthMap = 1,2,4 (1.Dim, 2.Dim, 3.Dim)
 * widths = 2,2,2,3,3,3,3 
 * elements = ...
 * nulls = 0000.1000, 0000.0000 (bitstrings - from left to right) 
 */
class Array {
    private:
    // The type of the array elements (only necessary for printing).
    uint8_t type;
    // The number of dimensions in this array.
    uint32_t dimensions;
    // The number of elements in this array (without NULL values).
    uint32_t size;
    // A pointer to the first index values of each dimension.
    int32_t *indices;
    // A pointer to the number of width values for each dimension.
    uint32_t *dimensionWidthMap;
    // A pointer to the widths of each array structure.
    uint32_t *widths;
    // A pointer to the first element of this array.
    uint8_t *elements;
    // A pointer to the first bitstring, representing NULL values.
    uint8_t *nulls;
    // A pointer to the first character of a string (if array stores strings).
    char *strings;

/*##########################################################################################################################################################  
 *                                                              PRIVATE METHODS
 *##########################################################################################################################################################*/    
 
    /**
     * This function copies the specified data into the provided target.
     * 
     * @param buffer A reference to a char pointer which points to the string
     * that should store the result.
     * @param data A pointer to the data that should be copied.
     * @param size The number of elements that should be copied from the given
     * data pointer.
     */
    template<class TYPE>
    static void writeToBuffer(char *&buffer, const TYPE *data, uint32_t size);

    /**
     * This function casts a string into a value of the corresponding type `TYPE` and
     * copy its value to the given pointer.
     * 
     * @param buffer A reference to the pointer in which the casted element should be copied.
     * @param value A reference to the string which should be casted.
     * @throws `std::runtime_error`: If the value could not be casted or if the resulting
     * value is out of range.
     */
    template<class TYPE>
    static void castAndCopyElement(char *&buffer, std::string &value);

    /**
     * This function casts the array elements at a certain position to the provided type
     * and copies the result to the buffer.
     * 
     * @param buffer A reference to the pointer in which the casted element should be copied.
     * @param position The position of the element that should be casted.
     * Possible value range `[1:this->size-1]`.
     * @param type The result type of the array element.
     * @throws `std::runtime_error`: If the provided type is not supported in arrays. If the
     * provided position is out of bounds.
     */
    template<class TYPE>
    void castAndCopyElement(char *&buffer, uint32_t position, uint8_t type);

    /**
     * This function parses the given header in a string.
     * 
     * @param array A reference to the specified array as string.
     * @param indices A reference to a vector which should store the start indices.
     * @param lengths A reference to a vector which should store the total length of each dimension.
     * @throws `std::runtime_error`: If the header is a invalid specification. If the header has 
     * incorrect syntax. If the header value could not be casted to a 32-bit integer.
     * @return The number of characters which corresponds to the header.
     */
    static uint32_t parseHeader(std::string &array, std::vector<int32_t> &indices, std::vector<uint32_t> &lengths);

    /**
     * This function calculates the necessary size of a string to store all data of the array.
     * 
     * @param dimensions The number of dimensions.
     * @param size The number of elements (without NULL).
     * @param widths The number of widths elements.
     * @param nullSize The number of bitstrings, representing NULL values.
     * @param stringSize The total length of all string elements (if array stores string values).
     * @param type The type of each array element.
     * @throws `std::runtime_error`: If the provided type is not supported.
     */
    static size_t getStringSize(uint32_t dimensions, uint32_t size, uint32_t widths, uint32_t nullSize, uint32_t stringSize, uint8_t type);
    
    /**
     * This function returns the size of a specific element type.
     * 
     * @param type The type of each array element.
     * @throws `std::runtime_error`: If the type is not supported.
     * @return The size of a single element of the provided type.
     */
    static size_t getTypeSize(uint8_t type);

    /**
     * This function maps an integer 32-bit type specification to
     * an unsigned integer 8-bit type specification.
     * 
     * @param type The enumeration value of the type.
     * @throws `std::runtime_error`: If the provided value is not 
     * listed in the type enumeration for arrays.
     */
    static uint8_t getTypeId(int32_t type);

    /**
     * This method returns the exact element position of a value stored in the 
     * element section of an array.
     * 
     * @param position The absolute position of the element (including NULL values).
     * @throws `std::runtime_error`: If the given position is out of range.
     * @return The relative position of the element (excluding NULL values).
     */
    uint32_t getElementPosition(uint32_t position);

    /**
     * This method executes a slice operation by storing important array information in the
     * provided vector elements (This method will be called recursively).
     * 
     * @param widths A reference to a vector that stores the updated width entries.
     * @param widthSize A reference to a vector that stores the updated dimensionWidthMap entries.
     * This vector must be filled.
     * @param elements A reference to a vector that stores the position of each element 
     * that should remain (including NULL values).
     * @param lowerBound The index of the first element that should remain.
     * @param upperBound The index of the last element that should remain.
     * @param sliceDimension The dimension in which the slice operation should happen.
     * @param dimension The current dimension.
     * @param entry A pointer to the width element that is processed.
     */
    void getArraySlice(
        std::vector<uint32_t> &widths, 
        std::vector<uint32_t> &widthSize, 
        std::vector<uint32_t> &elements, 
        int32_t lowerBound, 
        int32_t upperBound, 
        uint32_t sliceDimension, 
        uint32_t dimension,
        const uint32_t *&entry);

    /**
     * This method casts an element at the given position to a string (only if value
     * is not of type string) and append it to the end of the target string.
     * 
     * @param position The position of the array element. Possible value range 
     * `[0:numberElements-1]`.
     * @throws `std::runtime_error`: If the given position is out of range.
     */
    template<class TYPE>
    void toString(uint32_t position, std::string &target);

    /**
     * This method appends a single element of type `TYPE` to the last array structure
     * (lowest dimension). This method should only be used to append primitive elements.
     * 
     * @param value The value which should be appended.
     * @return The extended array in array processable string format.
     */
    template<class TYPE>
    VarLen32 appendElement(TYPE value);

    /**
     * This method appends a single element of type `TYPE` to the first array structure
     * (lowest dimension). This method should only be used to append primitive elements.
     * 
     * @param value The value which should be appended.
     * @return The extended array in array processable string format.
     */
    template<class TYPE>
    VarLen32 appendElementFront(TYPE value);

    /**
     * This method executes a specified binary scalar operation (`OP`).
     * 
     * @param value The scalar value of type `TYPE`. Should be numeric type.
     * @param isLeft If the scalar is on the left side of the operation.
     * @return The result of the binary scalar operation as string in array
     * processable format. 
     */
    template<class TYPE, class OP>
    VarLen32 executeScalarOperation(TYPE value, bool isLeft);

    /**
     * This method executes a specified activation function (`OP`).
     * 
     * @return The result of the activation function as string in array
     * processable format.
     */
    template<class OP>
    VarLen32 executeActivationFunction();

    /**
     * This function executes a specified binary function `OP` with numeric values.
     * 
     * @param left A pointer to the value of the first parameter of the binary function.
     * @param right A pointer to the value of the second parameter of the binary function.
     * @param size The length of both value lists.
     * @param buffer A reference to a char pointer which points to the string
     * that should store the result.
     * @param scalarLeft If the left parameter points to a single element.
     * @param scalarRight If the right parameter points to a single element. 
     * @param type The element type.
     * @throws `std::runtime_error`: If the array type is not supported in this function.
     */
    template<class OP>
    static void executeBinaryOperation(const uint8_t *left, const uint8_t *right, uint32_t size, char *&buffer, bool scalarLeft, bool scalarRight, uint8_t type);

    /**
     * This function executes a specified unary function `OP` with numeric values.
     * 
     * @param data A pointer the first element of a list.
     * @param size The length of the list.
     * @param buffer A reference to a char pointer which points to the string
     * that should store the result.
     * @param type The element type.
     * @throws `std::runtime_error`: If the array type is not supported in this function.
     */
    template<class OP>
    static void executeUnaryOperation(const uint8_t *data, uint32_t size, char *&buffer, uint8_t type);

    /**
     * This function generates an array with the given value and structure.
     * 
     * @param value A pointer to the value which represents the array element.
     * @param structure A reference to an array which specifies the resulting
     * structure. NULL values and multiple dimensions will be ignored.
     * @param type The type of the array elements.
     * @param stringSize The length of the string element (if value is of type `char`)
     * @return The resulting array as string in array processable format.
     */
    template<class TYPE, class ARRAYTYPE>
    static VarLen32 generate(TYPE *value, Array &structure, uint8_t type, uint32_t stringSize = 0);

    /**
     * This function generates an array with the given structure filled with NULL values.
     * 
     * @param structure A reference to an array which specifies the resulting
     * structure. NULL values and multiple dimensions will be ignored.
     * @param type The type of the array elements.
     * @return The resulting array as string in array processable format.
     */
    template<class ARRAYTYPE>
    static VarLen32 generate(Array &structure, uint8_t type);

    /**
     * This method returns the index of the largest value stored in this array.
     */
    template<class TYPE>
    int32_t getMaxIndex();

    /**
     * This method returns the size of a particular dimension by returning
     * the largest width in that dimension.
     * 
     * @param dimension The requested dimension. Possible value range 
     * `[1:numberDimensions]`.
     * @throws `std::runtime_error`: If the dimension is out of range.
     */
    uint32_t getDimensionSize(uint32_t dimension);

    /**
     * This method returns the index of the first element that
     * belongs to the provided width entry.
     * 
     * @param width A pointer to a width entry of the last dimension.
     * @throws `std::runtime_error`: If the provided width could not be
     * found in the width entries of the last dimension.
     */
    uint32_t getOffset(const uint32_t *width);

    /**
     * This method transforms the array into its string representation (for printing).
     * This method will be called recursively over each width entry.
     * 
     * @param target A reference to a string in which the result should be stored.
     * @param width A pointer to the width element that is processed.
     * @param dimension The current dimension.
     * @throws `std::runtime_error`: If the provided array structure is
     * not valid (processable).
     */
    void printArray(std::string &target, const uint32_t *width, uint32_t dimension);

    /**
     * This method prints the stored header to the target string, if at least a single
     * stored index is not the default value (1).
     * 
     * @param target A reference to the target string to which the header should be appended. 
     */
    void printHeader(std::string &target);

    /**
     * This method proofs if the given position is a NULL value.
     * 
     * @param position The position of the array element.
     * @throws `std::runtime_error`: If the provided position is out of range.
     * @return `True` if there is a NULL value, otherwise `False`.
     */
    bool isNull(uint32_t position);

    /**
     * This method initializes every attribute of this class except the type 
     * attribute.
     * 
     * @param data A pointer to the string that stores the array in array 
     * processable format.
     */
    void initArray(char *data);

    /**
     * This method cast each element of the array into the specified type.
     * 
     * @param type The type in which each element should casted.
     * @throws `std::runtime_error`: If the provided type is not numeric. If the
     * array element could not be casted to the provided type.
     * @return The resulting array as string in array processable format.
     */
    VarLen32 castToNumeric(uint8_t type);

    /**
     * This method cast each element of the array into strings.
     * 
     * @return The resulting array as string in array processable format.
     */
    VarLen32 castToString();

    // DEBUG-METHODS
    void printData();
    void printNulls();

/*##########################################################################################################################################################  
*                                                              PUBLIC METHODS
*##########################################################################################################################################################*/

    public:

    // This enumeration specifies all array element types
    enum ArrayType {
        INTEGER32,
        INTEGER64,
        BFLOAT,
        FLOAT,
        DOUBLE,
        STRING,
    };

    // This constant defines the first characters of an array (necessary for printing)
    static const std::string ARRAYHEADER;

    /**
     * This constructor generates an array object by extracting all data from the given string.
     * The given string should be in a processible array format. Otherwise this can lead to
     * undefined behaviour. Is the string not in a processable format use the `fromString` function.
     * @param array A reference to the string which stores the array data.
     * @param type The enum (`ArrayType`) value of the element type.
     * @throws `std::runtime_error`: If the given string is empty or does not include the array 
     * identification header. 
     */
    Array(std::string &array, int32_t type);

    /**
     * This constructor generates an array object by extracting all data from the given string.
     * The given string should be in a processible array format. Otherwise this can lead to
     * undefined behaviour. Is the string not in a processable format use the `fromString` function.
     * @param array A reference to the string which stores the array data.
     * @throws `std::runtime_error`: If the given string is empty or does not include the array 
     * identification header. 
     */
    Array(std::string &array);

    /**
     * This function parses the raw string into a processible array format.
     * 
     * @param source A reference to the source string.
     * @param type The type of each array element (enumeration value).
     * @throws `std::runtime_error`: If the given string has invalid syntax. If the
     * provided elements could not be casted to the specified type. If the provided
     * type is not supported.
     */
    static VarLen32 fromString(std::string &source, int32_t type);

    /**
     * This function creates an empty array.
     * 
     * @param type The type of the array elements.
     * @throws `std::runtime_error`: If the provided type is not supported.
     * @return An empty array as string in array processable format.
     */
    static VarLen32 createEmptyArray(int32_t type);

    /**
     * This method copies all array elements in this array into the provided buffer.
     * 
     * @param buffer A reference to the string buffer where the content needs to be stored.
     */
    void copyElements(char *&buffer);

    /**
     * This method copies the array element on the given position into the provided buffer.
     * 
     * @param buffer A reference to the string buffer where the content needs to be stored.
     * @param position  The position of the element. Possible value range `[0:size-1]`. 
     * Position values should not include NULL values.
     * @throws `std::runtime_error`: If the position is out of range.
     */
    void copyElement(char *&buffer, uint32_t position);

    /**
     * This method copies all strings in this array into the provided buffer.
     * 
     * @param buffer A reference to the string buffer where the content needs to be stored.
     */
    void copyStrings(char *&buffer);

    /**
     * This method copies the string on the given position into the provided buffer.
     * 
     * @param buffer A reference to the string buffer where the content needs to be stored.
     * @param position The position of the element. Possible value range `[0:size-1]`. 
     * Position values should not include NULL values.
     * @throws `std::runtime_error`: If the position is out of range.
     */
    void copyString(char *&buffer, uint32_t position);

    /**
     * This method copies the given null bits into the provided buffer.
     * 
     * @param buffer A reference to the string buffer where the content needs to be stored.
     * @param nulls  A pointer to the null bitstrings that should be copied.
     * @param size The number of elements in this array (including NULL values).
     * @param position The start position of the first element. 
     */
    void copyNulls(char *&buffer, const uint8_t *nulls, uint32_t size, uint32_t position);

    /**
     * This function copies the boolean values from the given vector into the buffer.
     * Thereby each value is transformed into a single bit and stored in a bitstring (with size 8-bit). 
     * The order is from left to right. 
     * 
     * @param buffer A reference to the string buffer where the content needs to be stored.
     * @param nulls A reference to the boolean vector.
     */
    static void copyNulls(char *&buffer, std::vector<bool> &nulls);

    /**
     * This function counts the number of NULL values up to the specified position.
     * 
     * @param position The limit up to which position should be counted.
     * Possible value range `[0:size + nulls - 1]`.
     * @throws `std::runtime_error`: If the selected position is out of range.
     * @return The number of NULL values.
     */
    uint32_t countNulls(uint32_t position);

    /**
     * This method returns the type of all array elements.
     */
    uint8_t getType();

    /**
     * This method returns the dimension value of this array.
     */
    uint32_t getDimension();

    /**
     * This method returns the size of the array by returning
     * the number of elements stored.
     * 
     * @param withNulls If NULL values should be included.
     */
    uint32_t getSize(bool withNulls = false);

    /**
     * This method returns the total number of width entries.
     */
    uint32_t getWidthSize();

    /**
     * This method returns the number of width entris of a specific
     * dimension.
     * 
     * @param dimension The selected dimension. Possible value range `[1:dimensions]`.
     * @throws `std::runtime_error`: If the selected dimension is out of range.
     */
    uint32_t getWidthSize(uint32_t dimension);

    /**
     * This method returns the total string length of all strings in
     * this array. If the array type is not string, this method will return `0`. 
     */
    uint32_t getStringLength();

    /**
     * This method returns the string length of a particular element.
     * 
     * @param position The position of the element. Possible value range
     * `[0:size-1]`. Position values should not include NULL values.
     * @throws `std::runtime_error`: If the selected position is out of range.
     * @return A string length if the array stores strings, otherwise `0`.
     */
    uint32_t getStringLength(uint32_t position);

    /**
     * This method returns a pointer to the width entries.
     */
    const uint32_t* getWidths();

    /**
     * This method returns a pointer to the array elements.
     */
    const uint8_t* getElements();

    /**
     * This method returns a pointer to the null bitstrings.
     */
    const uint8_t* getNulls();

    /**
     * This function returns the number of bytes required to store NULL bits
     * for each position.
     * 
     * @param size The number of elements that requires a NULL bit.
     * @return Number of NULL bytes.
     */
    static uint32_t getNullBytes(uint32_t size);

    /**
     * This method returns a pointer to the first width entry that
     * belongs to the provided dimension.
     * 
     * @param dimension The selected dimension. Possible value range `[1:dimensions]`.
     * @throws `std::runtime_error`: If the selected dimension is out of range.
     */
    const uint32_t* getFirstWidth(uint32_t dimension);

    /**
     * This method returns a pointer to the first width entry that
     * belongs as child to the given width entry.
     * 
     * @param width A pointer to the width entry.
     * @param dimension The dimension of the given width entry. 
     * Possible value range `[1:dimensions]`.
     * @throws `std::runtime_error`: If the selected dimension is out of range.
     * @return A pointer to the first child width entry. If not any child 
     * could be found it will return a `nullptr`.
     */
    const uint32_t* getChildWidth(const uint32_t *width, uint32_t dimension);

    /**
     * This method returns the index of the largest value in this array.
     * 
     * @throws `std::runtime_error`: If the given element type is not supported.
     */
    int32_t getHighestPosition();

    /**
     * This method proofs if the given width pointer has the same structure
     * (equal elements) as the width attribute from this array.
     * 
     * @param other A pointer to the first width entry of another array.
     * @return `True` if each width entry is equal, otherwise `False`.
     */
    bool equalWidths(const uint32_t *other);

    /**
     * This method proofs if the array contains NULL values.
     * 
     * @return `True` if a NULL value has been found, otherwise `False`.
     */
    bool hasNullValue();

    /**
     * This method proofs if the array contains an empty array structure.
     * 
     * @return `True` if an empty array structure has been found, otherwise `False`.
     */
    bool hasEmptyValue();

    /**
     * This method proofs if this array is empty.
     */
    bool isEmpty();

    /**
     * This method proofs if the array is symmetric. This means that each dimension
     * must have equal sizes.
     * 
     * @return `True` if the array is symmetric in each dimension, otherwise `False`.
     */
    bool isSymmetric();

    /**
     * This method proofs if the element type is numeric.
     * 
     * @param type The type that should be checked.
     * 
     * @return `True` if the element type is numeric, otherwise `False`.
     */
    static bool isNumericType(uint8_t type);

    /**
     * This method proofs if the element type is a floating point type.
     * 
     * @param type The type that should be checked.
     * 
     * @return `True` if the element type is a floating point type, otherwise `False`.
     */
    static bool isFloatingPointType(uint8_t type);

    /**
     * This function appends a structure or value of type `TYPE` to the array. Thereby the content
     * will be appended to the last array element. This depends on the dimension structure of the
     * parameter.
     * 
     * @param toAppend A reference to the structure or value that should be appended.
     * @throws `std::runtime_error`: If the structure to be appended has more dimensions or a 
     * different element type. If the value to be appended of a different type.
     * @return The string in array processable format storing the extended array. 
     */
    template<class TYPE> 
    VarLen32 append(TYPE &toAppend);

    /**
     * This function appends a structure or value of type `TYPE` to the array. Thereby the content
     * will be appended to the first array element. This depends on the dimension structure of the
     * parameter.
     * 
     * @param toAppend A reference to the structure or value that should be appended.
     * @throws `std::runtime_error`: If the structure to be appended has more dimensions or a 
     * different element type. If the value to be appended of a different type.
     * @return The string in array processable format storing the extended array. 
     */
    template<class TYPE>
    VarLen32 appendFront(TYPE &toAppend);

    /**
     * This function appends a new NULL value to the last array structure that stores single
     * elements.
     * 
     * @return The string in array processable format storing the extended array. 
     */
    VarLen32 append();

    /**
     * This method executes a slice operation.
     * 
     * @param lowerBound The index of the first element that should remain.
     * @param upperBound The index of the last element that should remain.
     * @param dimension The dimension in which the slice operation should take place.
     * @throws `std::runtime_error`: If the given lowerBound is larger than the given upperBound.
     * @return The modified array after the slice operation as string in array processable format. 
     * If the given lowerBound and upperBound are out of range, this method will return an empty 
     * array.
     */
    VarLen32 slice(int32_t lowerBound, int32_t upperBound, uint32_t dimension);

    /**
     * This method executes the subscript operator to get an element of this array.
     * 
     * @param position The index of the array element that should be returned.
     * @return If the array has more than one dimension, it returns another array. If the
     * array has only a single dimension, it returns the element as string. If no
     * element could be found, it returns an empty string (that should be mapped to NULL).
     */
    VarLen32 operator[](uint32_t position);

    /**
     * This method executes elementwise addition on each element.
     * 
     * @param other A reference to the array whose values are to be used for the addition.
     * @throws `std::runtime_error`: If the array type is not numeric. If both arrays have
     * different types. If one array has NULL values or empty array structures. If both
     * arrays have unequal array structures (unequal widths).
     * @return The result array as string in array processable format.
     */
    VarLen32 operator+(Array &other);

    /**
     * This method executes elementwise subtraction on each element.
     * 
     * @param other A reference to the array whose values are to be used for the subtraction.
     * @throws `std::runtime_error`: If the array type is not numeric. If both arrays have
     * different types. If one array has NULL values or empty array structures. If both
     * arrays have unequal array structures (unequal widths).
     * @return The result array as string in array processable format.
     */
    VarLen32 operator-(Array &other);

    /**
     * This method executes elementwise multiplication on each element.
     * 
     * @param other A reference to the array whose values are to be used for the multiplication.
     * @throws `std::runtime_error`: If the array type is not numeric. If both arrays have
     * different types. If one array has NULL values or empty array structures. If both
     * arrays have unequal array structures (unequal widths).
     * @return The result array as string in array processable format.
     */
    VarLen32 operator*(Array &other);

    /**
     * This method executes elementwise division on each element.
     * 
     * @param other A reference to the array whose values are to be used for the division.
     * @throws `std::runtime_error`: If the array type is not numeric. If both arrays have
     * different types. If one array has NULL values or empty array structures. If both
     * arrays have unequal array structures (unequal widths).
     * @return The result array as string in array processable format.
     */
    VarLen32 operator/(Array &other); 

    /**
     * This method executes scalar addition. All elements of the array will be added
     * with the provided value.
     * 
     * @param value The scalar value of type `TYPE`.
     * @throws 'std::runtime_error': If the array does not store values of type `TYPE`.
     * @return The result array as string in array processable format.
     */
    template<class TYPE>
    VarLen32 scalarAdd(TYPE value);

    /**
     * This method executes scalar subtraction. All elements of the array will be subtracted
     * with the provided value.
     * 
     * @param value The scalar value of type `TYPE`.
     * @param isLeft If the scalar is on the left side of the operation.
     * @throws 'std::runtime_error': If the array does not store values of type `TYPE`.
     * @return The result array as string in array processable format.
     */
    template<class TYPE>
    VarLen32 scalarSub(TYPE value, bool isLeft);

    /**
     * This method executes scalar multiplication. All elements of the array will be multiplied
     * with the provided value.
     * 
     * @param value The scalar value of type `TYPE`.
     * @throws 'std::runtime_error': If the array does not store values of type `TYPE`.
     * @return The result array as string in array processable format.
     */
    template<class TYPE>
    VarLen32 scalarMul(TYPE value);

    /**
     * This method executes scalar division. All elements of the array will be divided
     * with the provided value.
     * 
     * @param value The scalar value of type `TYPE`.
     * @param isLeft If the scalar is on the left side of the operation.
     * @throws 'std::runtime_error': If the array does not store values of type `TYPE`.
     * @return The result array as string in array processable format.
     */
    template<class TYPE>
    VarLen32 scalarDiv(TYPE value, bool isLeft);

    /**
     * This method executes matrix multiplication.
     * 
     * @param other The second matrix representing the right operand.
     * @throws `std::runtime_error`: If one of the following points is true:
     * - If the array element type is not a floating point type.
     * - If both arrays have different types.
     * - If NULL values are identified.
     * - If empty array structures are identified.
     * - If both arrays are not symmetric in each dimension.
     * - If both array structures does not allow matrix multiplication.
     * @return The result array as string in array processable format.
     */
    VarLen32 matrixMul(Array &other);
    
    /**
     * This method transposes the arrays dimension. In particular the first two dimensions
     * will be swapped which leads to a rearrangement of the elements. 
     * @note An array with a single dimension will be transformed into an array with 
     * two dimensions. For all other cases, the number of dimensions remains the same.
     * @throws `std::runtime_error`: If the element type is not numeric. If the provided array 
     * is not symmetric or contains empty array structures.
     * @return The transposed array as string in array processable format.
     */
    VarLen32 transpose();

    /**
     * This function generates an array with the given structure filled with the given
     * value. 
     * 
     * @param value A reference to the value which represents the array element. Defines
     * the type of the resulting array.
     * @param structure A reference to an array that specifies the dimension structure of
     * the resulting array. NULL values, empty array elements and multiple dimensions will
     * be ignored.
     * @throws 'std::runtime_error': If the array structure parameter does not store integer
     * values. If the array structure parameter does not contain any elements.
     * @return The generated array as string in array processable format.
     */
    template<class TYPE>
    static VarLen32 fill(TYPE &value, Array &structure);

    /**
     * This function generates an array with the given structure filled with NULL values.
     * 
     * @param structure A reference to an array that specifies the dimension structure of
     * the resulting array. NULL values, empty array elements and multiple dimensions will
     * be ignored.
     * @throws 'std::runtime_error': If the array structure parameter does not store integer
     * values. If the array structure parameter does not contain any elements.
     * @return The generated array as string in array processable format.
     */
    static VarLen32 fill(Array &structure);

    /**
     * This function executes the sigmoid activation function over every element in the array.
     * 
     * @throws 'std::runtime_error': If the element type is not numeric.
     * @return The result of the activation function as string in array
     * processable format.
     */
    VarLen32 sigmoid();

    /**
     * This function casts the provided array into an new array with another type.
     * 
     * @param type The result type of each array element.
     * @throws `std::runtime_error`: If the provided type is not supported. If the array
     * elements could not be casted to the provided type.
     */
    VarLen32 cast(int32_t type);

    /**
     * This function increases the total number of dimensions by one. The current
     * content of this array is wrapped into a new dimension.
     * 
     * @return The array with increased dimension as string in array
     * processable format.
     */
    VarLen32 increment();

    /**
     * This function converts the given string into an VarLen32.
     * 
     * @param str The string that should be converted.
     * @return The VarLen32 object containing the given string.
     */
    static VarLen32 toVarLen32(std::string &str);

    /**
     * This function transforms the array in a string that can be printed to the console.
     */
    std::string print();

};

#include "runtime/Array.tpp"

}
#endif