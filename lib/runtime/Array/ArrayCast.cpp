#include "runtime/Array.h"

using runtime::Array;

runtime::VarLen32 Array::cast(int32_t type) {
    auto typeId = getTypeId(type);
    if (typeId == ArrayType::STRING) return castToString();
    return castToNumeric(typeId);
}

runtime::VarLen32 Array::castToNumeric(uint8_t type) {
    // Check if type is numeric
    if (!isNumericType(type)) {
        throw std::runtime_error("Array-Cast: Provided type is not numeric");
    }
    // Create result string and copy each metadata to it (keeps the same)
    auto totalElements = getSize(true);
    std::string result;
    auto size = getStringSize(this->dimensions, this->size, getWidthSize(), getNullBytes(totalElements), 0, type);
    result.resize(size);
    char *buffer = result.data();

    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &type, 1);
    writeToBuffer(buffer, &this->dimensions, 1);
    writeToBuffer(buffer, &this->size, 1);
    writeToBuffer(buffer, this->indices, this->dimensions);
    writeToBuffer(buffer, this->dimensionWidthMap, this->dimensions);
    writeToBuffer(buffer, this->widths, getWidthSize());

    // Iterate over each element and cast it to the provided type
    for (uint32_t i = 0; i < this->size; i++) {
        switch (this->type) {
            case ArrayType::INTEGER32:
                castAndCopyElement<int32_t>(buffer, i, type);
                break;
            case ArrayType::INTEGER64:
                castAndCopyElement<int64_t>(buffer, i, type);
                break;
            case ArrayType::BFLOAT:
                castAndCopyElement<__bf16>(buffer, i, type);
                break;
            case ArrayType::FLOAT:
                castAndCopyElement<float>(buffer, i, type);
                break;
            case ArrayType::DOUBLE:
                castAndCopyElement<double>(buffer, i, type);
                break;
            case ArrayType::STRING:
                castAndCopyElement<uint32_t>(buffer, i, type);
                break;
            default:
                throw std::runtime_error("Numeric-Cast-Operation: Given array type is not supported");
        }
    }
    copyNulls(buffer, this->nulls, totalElements, 0);
    return toVarLen32(result);
}

runtime::VarLen32 Array::castToString() {
    auto totalElements = getSize(true);
    std::vector<std::string> elements;

    // Iterate over each element and cast it to a string
    uint32_t offset = 0;
    for (uint32_t i = 0; i < this->size; i++) {
        switch (this->type) {
            case ArrayType::INTEGER32: {
                auto *values = reinterpret_cast<int32_t *>(this->elements);
                elements.push_back(std::to_string(values[i]));
                break;
            }
            case ArrayType::INTEGER64: {
                auto *values = reinterpret_cast<int64_t *>(this->elements);
                elements.push_back(std::to_string(values[i]));
                break;
            }
            case ArrayType::BFLOAT: {
                auto *values = reinterpret_cast<__bf16 *>(this->elements);
                elements.push_back(std::to_string(static_cast<float>(values[i])));
                break;
            }
            case ArrayType::FLOAT: {
                auto *values = reinterpret_cast<float *>(this->elements);
                elements.push_back(std::to_string(values[i]));
                break;
            }
            case ArrayType::DOUBLE: {
                auto *values = reinterpret_cast<double *>(this->elements);
                elements.push_back(std::to_string(values[i]));
                break;
            }
            case ArrayType::STRING: {
                auto *values = reinterpret_cast<uint32_t *>(this->elements);
                elements.push_back(std::string(this->strings + offset, values[i]));
                offset += values[i];
                break;
            }
            default:
                throw std::runtime_error("String-Cast-Operation: Given array type is not supported");
        }
    }

    // Calculate total string length
    uint32_t lengths = 0;
    for (auto &entry : elements) {
        lengths += entry.length();
    }

    // Create result string and copy each metadata to it (keeps the same)
    std::string result;
    uint8_t type = ArrayType::STRING;
    auto size = getStringSize(this->dimensions, this->size, getWidthSize(), getNullBytes(totalElements), lengths, type);
    result.resize(size);
    char *buffer = result.data();

    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &type, 1);
    writeToBuffer(buffer, &this->dimensions, 1);
    writeToBuffer(buffer, &this->size, 1);
    writeToBuffer(buffer, this->indices, this->dimensions);
    writeToBuffer(buffer, this->dimensionWidthMap, this->dimensions);
    writeToBuffer(buffer, this->widths, getWidthSize());

    for (auto &entry : elements) {
        uint32_t length = entry.length();
        writeToBuffer(buffer, &length, 1);
    }

    copyNulls(buffer, this->nulls, totalElements, 0);
    for (auto &entry : elements) {
        writeToBuffer(buffer, entry.data(), entry.length());
    }
    return toVarLen32(result);
}