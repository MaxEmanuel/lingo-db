#include "runtime/Array.h"

using runtime::Array;

runtime::VarLen32 Array::operator[](uint32_t position) {

    std::string result = "";
    // These variables store updated array data
    std::vector<uint32_t> widths;
    std::vector<uint32_t> widthSize;
    std::vector<uint32_t> elementIdx;
    std::vector<bool> nulls;
    uint32_t numberElements = 0;
    uint32_t stringLengths = 0;

    // Size needs to be same as in current array
    widthSize.resize(this->dimensions, 0);
    
    const auto *start = this->widths;
    // Use slice function to execute subscript
    getArraySlice(widths, widthSize, elementIdx, position, position, 1, 1, start);
    // Delete first width, because subscript reduce dimensions
    widthSize.erase(widthSize.begin());
    nulls.reserve(elementIdx.size());

    // If no element could be found, return empty string (TODO: Inside database map to NULL)
    if (elementIdx.size() == 0 && widths.size() == 0) {
        return toVarLen32(result);
    }

    // If array has only a single dimension, return this element without array data.
    if (this->dimensions == 1) {
        // Return NULL as empty string
        if (isNull(elementIdx[0])) {
            return toVarLen32(result);
        }
        // Return element as string (return type must be fixed)
        auto position = getElementPosition(elementIdx[0]);
        if (type == ArrayType::INTEGER32) {
            toString<int32_t>(position, result);
        } else if (type == ArrayType::INTEGER64) {
            toString<int64_t>(position, result);
        } else if (type == ArrayType::BFLOAT) {
            toString<__bf16>(position, result);
        } else if (type == ArrayType::FLOAT) {
            toString<float>(position, result);
        } else if (type == ArrayType::DOUBLE) {
            toString<double>(position, result);
        } else {
            toString<std::string>(position, result);
        }
        return toVarLen32(result);
    }

    // Based on element indicies, fill NULL vector and string length vector
    for (auto &entry : elementIdx) {
        bool null = isNull(entry);
        nulls.push_back(null);
        if (type == ArrayType::STRING && !null) {
            stringLengths += getStringLength(getElementPosition(entry));
        }
        // Also calculate number of elements that are not NULL
        if (!null) numberElements++;
    }

    // Reduce dimension, if empty array structures exist
    auto dimensions = this->dimensions - 1;
    for (uint32_t i = dimensions; i > 0; i--) {
        if (widthSize[i-1] == 0) dimensions--;
    }

    // Define result string
    size_t size = getStringSize(
        dimensions,
        numberElements,
        widths.size(),
        getNullBytes(nulls.size()),
        stringLengths,
        type
    );
    result.resize(size);
    char *buffer = result.data();

    // Write array data into string
    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &this->type, 1);
    writeToBuffer(buffer, &dimensions, 1);
    writeToBuffer(buffer, &numberElements, 1);
    
    if (elementIdx.size() == 0) {
        // If empty array structures are returned, use default index
        uint32_t value = 1;
        for (uint32_t i = 0; i < dimensions; i++) {
            writeToBuffer(buffer, &value, 1);
        }
    } else {
        // Otherwise use original index
        writeToBuffer(buffer, &this->indices[1], dimensions);
    }

    writeToBuffer(buffer, widthSize.data(), dimensions);
    writeToBuffer(buffer, widths.data(), widths.size());

    for (auto &entry : elementIdx) {
        if (!isNull(entry)) {
            copyElement(buffer, getElementPosition(entry));
        }
    }
    copyNulls(buffer, nulls);

    if (type == ArrayType::STRING) {
        for (size_t i = 0; i < elementIdx.size(); i++) {
            auto j = elementIdx[i];
            if (!isNull(j)) {
                copyString(buffer, getElementPosition(j));
            }
        }
    }
    return toVarLen32(result);
}