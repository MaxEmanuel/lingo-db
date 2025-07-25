#include "runtime/Array.h"
#include <iostream>

using runtime::Array;

const std::string Array::ARRAYHEADER = "array";

Array::Array(std::string &array, int32_t type) {
    // Check if string is not empty
    if (array.size() == 0) {
        throw std::runtime_error("Array is empty");
    }
    
    char *data = array.data();
    std::string header(data, ARRAYHEADER.size());
    if (header != ARRAYHEADER) {
        throw std::runtime_error("Array is not processable");
    }
    auto typeId = getTypeId(type);
    data += ARRAYHEADER.size();
    data += 1;
    this->type = typeId;
    initArray(data);
}

Array::Array(std::string &array) {
    // Check if string is not empty
    if (array.size() == 0) {
        throw std::runtime_error("Array is empty");
    }
    
    char *data = array.data();
    std::string header(data, ARRAYHEADER.size());
    if (header != ARRAYHEADER) {
        throw std::runtime_error("Array is not processable");
    }
    data += ARRAYHEADER.size();
    this->type = *data;
    data += 1;
    initArray(data);
    /* if (type == ArrayType::INTEGER32 || type == ArrayType::FLOAT) {
        printData();
    } */
}

void Array::initArray(char *data) {
    // Assign each attribute
    this->dimensions = *reinterpret_cast<uint32_t*>(data);
    data += sizeof(uint32_t);
    this->size = *reinterpret_cast<uint32_t*>(data);
    data += sizeof(uint32_t);
    this->indices = reinterpret_cast<int32_t*>(data);
    data += sizeof(int32_t) * this->dimensions;
    this->dimensionWidthMap = reinterpret_cast<uint32_t*>(data);
    data += this->dimensions * sizeof(uint32_t);
    this->widths = reinterpret_cast<uint32_t*>(data);
    for (size_t i = 0; i < this->dimensions; i++) {
        data += this->dimensionWidthMap[i] * sizeof(uint32_t);
    }
    this->elements = reinterpret_cast<uint8_t*>(data);
    data += this->size * getTypeSize(type);
    this->nulls = reinterpret_cast<uint8_t*>(data);
    data += getNullBytes(getSize(true));
    this->strings = data;
}

runtime::VarLen32 Array::toVarLen32(std::string &str) {
    char* result = new char[str.length()];           
    memcpy(result, str.data(), str.length());     
    return runtime::VarLen32((uint8_t*) result, str.length());
}

runtime::VarLen32 Array::createEmptyArray(int32_t type) {
    auto typeId = getTypeId(type);
    std::string result;
    uint32_t dimension = 1;
    uint32_t elements = 0;
    auto size = getStringSize(1, 0, 1, 0, 0, typeId);
    result.resize(size);
    auto *buffer = &result[0];

    // Header
    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.size());
    // Type
    writeToBuffer(buffer, &typeId, 1);
    // Dimension
    writeToBuffer(buffer, &dimension, 1);
    // Size
    writeToBuffer(buffer, &elements, 1);
    // Indices
    writeToBuffer(buffer, &dimension, 1);
    // WidthMap
    writeToBuffer(buffer, &dimension, 1);
    // Widths
    writeToBuffer(buffer, &elements, 1);

    return toVarLen32(result);
}

runtime::VarLen32 Array::increment() {
    auto typeId = getTypeId(type);
    auto dimension = this->dimensions + 1;
    auto totalElements = getSize(true);
    auto stringLengths = getStringLength();
    std::string result;
    auto size = getStringSize(dimension, this->size, getWidthSize() + 1, getNullBytes(totalElements), stringLengths, this->type);
    result.resize(size);
    auto *buffer = result.data();

    uint32_t value = 1;
    // Header
    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.size());
    // Type
    writeToBuffer(buffer, &typeId, 1);
    // Dimension
    writeToBuffer(buffer, &dimension, 1);
    // Size
    writeToBuffer(buffer, &this->size, 1);
    // Indices
    writeToBuffer(buffer, &value, 1);
    writeToBuffer(buffer, this->indices, this->dimensions);
    // WidthMap
    writeToBuffer(buffer, &value, 1);
    writeToBuffer(buffer, this->dimensionWidthMap, this->dimensions);
    // Widths
    writeToBuffer(buffer, &value, 1);
    writeToBuffer(buffer, this->widths, getWidthSize());

    copyElements(buffer);
    copyNulls(buffer, this->nulls, totalElements, 0);
    copyStrings(buffer);

    return toVarLen32(result);
}

uint8_t Array::getTypeId(int32_t type) {
    switch (type)
    {
    case ArrayType::INTEGER32:
        return ArrayType::INTEGER32;
    case ArrayType::INTEGER64:
        return ArrayType::INTEGER64;
    case ArrayType::FLOAT:
        return ArrayType::FLOAT;
    case ArrayType::DOUBLE:
        return ArrayType::DOUBLE;
    case ArrayType::BFLOAT:
        return ArrayType::BFLOAT;
    case ArrayType::STRING:
        return ArrayType::STRING;
    default:
        throw std::runtime_error("Type-Cast-Operation: Given array type is not supported with arrays");
    }
}

bool Array::isNumericType(uint8_t type) {
    switch (type) 
    {
    case ArrayType::INTEGER32:
    case ArrayType::INTEGER64:
    case ArrayType::FLOAT:
    case ArrayType::DOUBLE:
    case ArrayType::BFLOAT:
        return true;
    }
    return false;
}

bool Array::isFloatingPointType(uint8_t type) {
    switch (type) 
    {
    case ArrayType::FLOAT:
    case ArrayType::DOUBLE:
    case ArrayType::BFLOAT:
        return true;
    }
    return false;
}

size_t Array::getStringSize(uint32_t dimensions, uint32_t size, uint32_t widths, uint32_t nullSize, uint32_t stringSize, uint8_t type) {
    size_t result = ARRAYHEADER.size() + 1;
    result += (sizeof(uint32_t) * 2) + (dimensions * sizeof(int32_t)) + (dimensions * sizeof(uint32_t)) + (widths * sizeof(uint32_t));
    result += size * getTypeSize(type);
    result += nullSize;
    result += stringSize;
    return result;
}

size_t Array::getTypeSize(uint8_t type) {
    switch (type) 
    {
    case ArrayType::INTEGER32:
        return sizeof(int32_t);
    case ArrayType::INTEGER64:
        return sizeof(int64_t);
    case ArrayType::FLOAT:
        return sizeof(float);
    case ArrayType::DOUBLE:
        return sizeof(double);
    case ArrayType::STRING:
        return sizeof(uint32_t);
    default:
        throw std::runtime_error("Type-Size-Operation: Given array type is not supported in arrays");
    }
}

uint8_t Array::getType() {
    return this->type;
}

uint32_t Array::getDimension() {
    return this->dimensions;
}

void Array::printData() {
    size_t metadataLen = 0;
    std::cout << "INDICES:" << std::endl;
    for (size_t i = 0; i < this->dimensions; i++) {
        std::cout << this->indices[i] << ",";
    }
    std::cout << std::endl;
    std::cout << "WIDTH-LENGTHS:" << std::endl;
    for (size_t i = 0; i < this->dimensions; i++) {
        std::cout << this->dimensionWidthMap[i] << ",";
        metadataLen += this->dimensionWidthMap[i];
    }
    std::cout << std::endl;
    std::cout << "WIDTH-ENTRIES:" << std::endl;
    for (size_t i = 0; i < metadataLen; i++) {
        std::cout << this->widths[i] << ","; 
    }
    std::cout << std::endl;
    printNulls();
}

void Array::printNulls() {
    std::cout << "NULLS:" << std::endl;
    for (size_t i = 0; i < getSize(true); i++) {
        std::cout << isNull(i) << ",";
    }
    std::cout << std::endl;
}