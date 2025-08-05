// This file contains implementations of template functions that have not been specialised
#include <type_traits>
#include "runtime/Array.h"

using runtime::Array;

template<class TYPE>
void Array::writeToBuffer(char *&buffer, const TYPE *data, uint32_t size) {
    memcpy(buffer, data, sizeof(TYPE) * size);
    buffer += sizeof(TYPE) * size;
}

template<class TYPE, class OP>
runtime::VarLen32 Array::executeScalarOperation(TYPE value, bool isLeft) {
    // Define result string size (does not change)
    auto totalElements = getSize(true);
    std::string result;
    auto size = getStringSize(this->dimensions, this->size, getWidthSize(), getNullBytes(totalElements), 0, type);
    result.resize(size);
    char *buffer = result.data();
    // Write every content to the result (does not change except elements)
    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &this->type, 1);
    writeToBuffer(buffer, &this->dimensions, 1);
    writeToBuffer(buffer, &this->size, 1);
    writeToBuffer(buffer, this->indices, this->dimensions);
    writeToBuffer(buffer, this->dimensionWidthMap, this->dimensions);
    writeToBuffer(buffer, this->widths, getWidthSize());
    auto *left = isLeft ? reinterpret_cast<const uint8_t*>(&value) : this->elements;
    auto *right = isLeft ? this->elements : reinterpret_cast<const uint8_t*>(&value);
    executeBinaryOperation<OP>(left, right, this->size, buffer, isLeft, !isLeft, this->type);
    copyNulls(buffer, this->nulls, totalElements, 0);
    return toVarLen32(result);
}

template<class OP>
runtime::VarLen32 Array::executeActivationFunction() {
    // Define result string size (does not change)
    auto totalElements = getSize(true);
    std::string result;
    auto size = getStringSize(this->dimensions, this->size, getWidthSize(), getNullBytes(totalElements), 0, type);
    result.resize(size);
    char *buffer = result.data();
    // Write every content to the result (does not change except elements)
    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &this->type, 1);
    writeToBuffer(buffer, &this->dimensions, 1);
    writeToBuffer(buffer, &this->size, 1);
    writeToBuffer(buffer, this->indices, this->dimensions);
    writeToBuffer(buffer, this->dimensionWidthMap, this->dimensions);
    writeToBuffer(buffer, this->widths, getWidthSize());
    auto *data = this->elements;
    executeUnaryOperation<OP>(data, this->size, buffer, this->type);
    copyNulls(buffer, this->nulls, totalElements, 0);
    return toVarLen32(result);
}

template<class OP>
void Array::executeBinaryOperation(const uint8_t *left, const uint8_t *right, uint32_t size, char *&buffer, bool scalarLeft, bool scalarRight, uint8_t type) {
    if (type == ArrayType::INTEGER32) {
        auto *leftVal = reinterpret_cast<const int32_t*>(left);
        auto *rightVal = reinterpret_cast<const int32_t*>(right);
        OP::Operator(leftVal, rightVal, size, buffer, scalarLeft, scalarRight);
    } else if (type == ArrayType::INTEGER64) {
        auto *leftVal = reinterpret_cast<const int64_t*>(left);
        auto *rightVal = reinterpret_cast<const int64_t*>(right);
        OP::Operator(leftVal, rightVal, size, buffer, scalarLeft, scalarRight);
    } else if (type == ArrayType::FLOAT) {
        auto *leftVal = reinterpret_cast<const float*>(left);
        auto *rightVal = reinterpret_cast<const float*>(right);
        OP::Operator(leftVal, rightVal, size, buffer, scalarLeft, scalarRight);
    } else if (type == ArrayType::DOUBLE) {
        auto *leftVal = reinterpret_cast<const double*>(left);
        auto *rightVal = reinterpret_cast<const double*>(right);
        OP::Operator(leftVal, rightVal, size, buffer, scalarLeft, scalarRight);
    } else {
        throw std::runtime_error("Array-Type is not supported");
    }
}

template<class OP>
void Array::executeUnaryOperation(const uint8_t *data, uint32_t size, char *&buffer, uint8_t type) {
    if (type == ArrayType::INTEGER32) {
        auto *values = reinterpret_cast<const int32_t*>(data);
        OP::Operator(values, size, buffer);
    } else if (type == ArrayType::INTEGER64) {
        auto *values = reinterpret_cast<const int64_t*>(data);
        OP::Operator(values, size, buffer);
    } else if (type == ArrayType::FLOAT) {
        auto *values = reinterpret_cast<const float*>(data);
        OP::Operator(values, size, buffer);
    } else if (type == ArrayType::DOUBLE) {
        auto *values = reinterpret_cast<const double*>(data);
        OP::Operator(values, size, buffer);
    } else {
        throw std::runtime_error("Array-Type is not supported");
    }
}

template<class TYPE>
runtime::VarLen32 Array::appendElement(TYPE value) {
    // Variables that will change in new array
    auto numberElements = this->size + 1;
    auto nullBytes = getNullBytes(getSize(true) + 1);
    auto widthSize = getWidthSize();
    auto lastWidth = this->widths[widthSize-1] + 1;

    std::string result;
    size_t size = getStringSize(this->dimensions, numberElements, widthSize, nullBytes, 0, this->type);
    result.resize(size);
    char *buffer = result.data();

    // Write complete content to result string
    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &this->type, 1);
    writeToBuffer(buffer, &this->dimensions, 1);
    writeToBuffer(buffer, &numberElements, 1);
    writeToBuffer(buffer, this->indices, this->dimensions);
    writeToBuffer(buffer, this->dimensionWidthMap, this->dimensions);
    writeToBuffer(buffer, this->widths, widthSize-1);
    writeToBuffer(buffer, &lastWidth, 1);
    copyElements(buffer);
    writeToBuffer(buffer, &value, 1);
    writeToBuffer(buffer, this->nulls, nullBytes);
    return toVarLen32(result);
}

template<class TYPE>
runtime::VarLen32 Array::appendElementFront(TYPE value) {
    // Variables that will change in new array
    auto numberElements = this->size + 1;
    auto nullBytes = getNullBytes(getSize(true) + 1);
    auto widthSize = getWidthSize();
    auto lastWidthSize = getWidthSize(this->dimensions);
    auto changedWidth = this->widths[widthSize - lastWidthSize] + 1;

    std::string result;
    size_t size = getStringSize(this->dimensions, numberElements, widthSize, nullBytes, 0, this->type);
    result.resize(size);
    char *buffer = result.data();

    // Write complete content to result string
    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &this->type, 1);
    writeToBuffer(buffer, &this->dimensions, 1);
    writeToBuffer(buffer, &numberElements, 1);
    writeToBuffer(buffer, this->indices, this->dimensions);
    writeToBuffer(buffer, this->dimensionWidthMap, this->dimensions);
    // Write widths from upper dimensions
    writeToBuffer(buffer, this->widths, widthSize - lastWidthSize);
    writeToBuffer(buffer, &changedWidth, 1);
    // Write not changed widths in last dimension
    writeToBuffer(buffer, this->widths + widthSize - lastWidthSize + 1, lastWidthSize-1);
    writeToBuffer(buffer, &value, 1);
    copyElements(buffer);
    writeToBuffer(buffer, this->nulls, nullBytes);
    return toVarLen32(result);
}

template<class TYPE, class ARRAYTYPE>
runtime::VarLen32 Array::generate(TYPE *value, Array &structure, uint8_t type, uint32_t stringSize) {
    const ARRAYTYPE *elements = reinterpret_cast<const ARRAYTYPE*>(structure.getElements());
    auto size = structure.getSize();

    // Get information about how many times the value must copied and
    // how many width entries are required in total and in each dimension 
    uint32_t elementCopies = 1;
    uint32_t widthSize = 1;
    std::vector<uint32_t> dimensionWidthMap{1};
    for (uint32_t i = 0; i < size; i++) {
        elementCopies *= static_cast<uint32_t>(elements[i]);
        if (i+1 < size) {
            dimensionWidthMap.push_back(elementCopies);
            widthSize += elementCopies; 
        }
    }

    // Prepare result string
    std::string result;
    auto resultSize = getStringSize(size, elementCopies, widthSize, getNullBytes(elementCopies), stringSize*elementCopies, type);
    result.resize(resultSize);
    char *buffer = result.data();

    // Write content to result
    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &type, 1);
    writeToBuffer(buffer, &size, 1);
    writeToBuffer(buffer, &elementCopies, 1);
    // Create new index values
    for (uint32_t i = 0; i < size; i++) {
        uint32_t index = 1;
        writeToBuffer(buffer, &index, 1);
    }
    writeToBuffer(buffer, dimensionWidthMap.data(), dimensionWidthMap.size());
    // Iterate over each element of the structure array
    for (uint32_t i = 0; i < size; i++) {
        auto width = static_cast<uint32_t>(elements[i]);
        auto length = dimensionWidthMap[i];
        // Iterate over each width entry that is required for the result array
        for (uint32_t j = 0; j < length; j++) {
            writeToBuffer(buffer, &width, 1);
        }
    }
    // Check if array element type is string
    if (!std::is_same<TYPE, char>::value){
        // If false, copy only n times the value
        for (uint32_t i = 0; i < elementCopies; i++) {
            writeToBuffer(buffer, value, 1);
        }
    } else {
        // If true, copy n times string length and n times the string itself
        for (uint32_t i = 0; i < elementCopies; i++) {
            writeToBuffer(buffer, &stringSize, 1);
        }
        buffer += getNullBytes(elementCopies);
        for (uint32_t i = 0; i < elementCopies; i++) {
            writeToBuffer(buffer, value, stringSize);
        }
    }

    return toVarLen32(result);
}

template<class ARRAYTYPE>
runtime::VarLen32 Array::generate(Array &structure, uint8_t type) {
    const ARRAYTYPE *elements = reinterpret_cast<const ARRAYTYPE*>(structure.getElements());
    auto size = structure.getSize();

    // Get information about how many times the value must copied and
    // how many width entries are required in total and in each dimension 
    uint32_t elementCopies = 1;
    uint32_t widthSize = 1;
    std::vector<uint32_t> dimensionWidthMap{1};
    for (uint32_t i = 0; i < size; i++) {
        elementCopies *= static_cast<uint32_t>(elements[i]);
        if (i+1 < size) {
            dimensionWidthMap.push_back(elementCopies);
            widthSize += elementCopies; 
        }
    }

    // Prepare result string
    std::string result;
    auto resultSize = getStringSize(size, 0, widthSize, getNullBytes(elementCopies), 0, type);
    result.resize(resultSize);
    char *buffer = result.data();

    uint32_t zero = 0;
    // Write content to result
    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &type, 1);
    writeToBuffer(buffer, &size, 1);
    writeToBuffer(buffer, &zero, 1);
    // Create new index values
    for (uint32_t i = 0; i < size; i++) {
        uint32_t index = 1;
        writeToBuffer(buffer, &index, 1);
    }
    writeToBuffer(buffer, dimensionWidthMap.data(), dimensionWidthMap.size());
    // Iterate over each element of the structure array
    for (uint32_t i = 0; i < size; i++) {
        auto width = static_cast<uint32_t>(elements[i]);
        auto length = dimensionWidthMap[i];
        // Iterate over each width entry that is required for the result array
        for (uint32_t j = 0; j < length; j++) {
            writeToBuffer(buffer, &width, 1);
        }
    }
    // Write null bits
    for (uint32_t i = 0; i < elementCopies; i++) {
        // Check if new null byte must be selected
        if (i != 0 && i % 8 == 0) {
            buffer++;
        }
        uint8_t index = i % 8;
        uint8_t shift = 8 - index - 1;
        *buffer |= (1 << shift);
    }

    return toVarLen32(result);
}

template<class TYPE>
int32_t Array::getMaxIndex() {
    auto *elements = reinterpret_cast<const TYPE*>(this->elements);
    int32_t result = -1;
    auto size = getSize(true);

    for (uint32_t i = 0; i < size; i++) {
        if (isNull(i)) continue;
        uint32_t position = getElementPosition(i);
        if (result == -1) result = i;
        uint32_t resultPosition = getElementPosition(result);
        if (elements[position] > elements[resultPosition]) result = i;
    }

    return result;
}

template<class TYPE>
void Array::castAndCopyElement(char *&buffer, uint32_t position, uint8_t type) {
    // Check if position is out of bounds
    if (position > this->size) {
        throw std::runtime_error("Requested array element does not exist");
    }
    // If current elements are strings, the actual value needs to be constructed
    if (this->type == ArrayType::STRING) {
        // The first character of the string
        uint32_t offset = 0;
        // The length of the string
        uint32_t length = 0;
        auto *lengths = reinterpret_cast<TYPE *>(this->elements);
        // Find first character and length
        for (uint32_t i = 0; i <= position; i++) {
            if (i == position) length = lengths[i];
            else offset += lengths[i];
        }
        // Construct string and try a cast to the provided type
        std::string value(this->strings + offset, length);
        switch (type) {
            case ArrayType::INTEGER32:
                castAndCopyElement<int32_t>(buffer, value);
                break;
            case ArrayType::INTEGER64:
                castAndCopyElement<int64_t>(buffer, value);
                break;
            case ArrayType::FLOAT:
                castAndCopyElement<float>(buffer, value);
                break;
            case ArrayType::DOUBLE:
                castAndCopyElement<double>(buffer, value);
                break;
            case ArrayType::STRING:
                castAndCopyElement<std::string>(buffer, value);
                break;
            default:
                throw std::runtime_error("Cast-Operation: Given array type is not supported");
        }
        return;
    }
    // If elements are not strings, get the value and do a simple cast
    // If the result type is string, cast the value to a string and copy its length
    auto *elements = reinterpret_cast<TYPE *>(this->elements);
    auto element = elements[position];
    switch (type) {
        case ArrayType::INTEGER32: {
            auto value = static_cast<int32_t>(element);
            writeToBuffer(buffer, &value, 1);
            break;
        }
        case ArrayType::INTEGER64: {
            auto value = static_cast<int64_t>(element);
            writeToBuffer(buffer, &value, 1);
            break;
        }
        case ArrayType::FLOAT: {
            auto value = static_cast<float>(element);
            writeToBuffer(buffer, &value, 1);
            break;
        }
        case ArrayType::DOUBLE: {
            auto value = static_cast<double>(element);
            writeToBuffer(buffer, &value, 1);
            break;
        }
        case ArrayType::STRING: {
            auto value = std::to_string(element);
            uint32_t length = value.length();
            writeToBuffer(buffer, &length, 1);
            break;
        }
        default:
            throw std::runtime_error("Cast-Operation: Given array type is not supported");
    }
}