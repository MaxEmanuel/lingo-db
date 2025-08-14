#include "runtime/Array.h"

using runtime::Array;

runtime::VarLen32 Array::transpose() {
    if (!isNumericType(this->type)) {
        throw std::runtime_error("Array-Tranypose: This function is only supported with numeric element types");
    }
    if (hasEmptyValue()) {
        throw std::runtime_error("Array-Transpose: This function does not allow any empty array structures");
    }
    if (!isSymmetric()) {
        throw std::runtime_error("Array-Transpose: This function allows only symmetric arrays");
    }

    // One dimensional arrays will increase their dimension, otherwise it remains
    auto dimension = this->dimensions == 1 ? 2 : this->dimensions;
    auto totalElements = getSize(true);
    auto widthSize = 0;
    if (this->dimensions == 1) widthSize = 1 + getWidthSize();
    // Remove old widthSize and replace it with new one
    else widthSize = getWidthSize() - getWidthSize(2) + this->widths[1];

    // Create result string in set its size
    std::string result;
    auto size = getStringSize(dimension, this->size, widthSize, getNullBytes(totalElements), 0, this->type);
    result.resize(size);
    char *buffer = result.data();

    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &this->type, 1);
    writeToBuffer(buffer, &dimension, 1);
    writeToBuffer(buffer, &this->size, 1);
    if (dimension == 1) {
        // Create new index
        uint32_t index = 1;
        writeToBuffer(buffer, &index, 1);
        writeToBuffer(buffer, this->indices, 1);
    } else {
        // Swap first and second index
        writeToBuffer(buffer, &this->indices[1], 1);
        writeToBuffer(buffer, &this->indices[0], 1);
        writeToBuffer(buffer, &this->indices[2], dimension-2);
    }
    auto *widthToChange = buffer + sizeof(uint32_t);
    uint32_t width = 1;
    writeToBuffer(buffer, this->dimensionWidthMap, this->dimensions);
    // If one dimensional it will get further width entries, because it will change to two dimensional
    if (this->dimensions == 1) writeToBuffer(buffer, &width, 1);
    // If two dimensional, override the width size with the new value
    else writeToBuffer(widthToChange, &this->widths[1], 1);

    if (this->dimensions == 1) {
        // Create new width and copy old one
        writeToBuffer(buffer, &width, 1);
        writeToBuffer(buffer, &this->widths[0], 1);
    } else {
        // Swap first and second width
        writeToBuffer(buffer, &this->widths[1], 1);
        for (uint32_t i = 0; i < this->widths[1]; i++) {
            writeToBuffer(buffer, &this->widths[0], 1);
        }
        if (this->dimensions > 2) {
            // Copy unchanged widths
            writeToBuffer(buffer, &this->widths[this->dimensionWidthMap[1]+1], getWidthSize() - (getWidthSize(1) + getWidthSize(2)));
        }
    }

    // If one-dimensional, copy all elements and nulls
    if (this->dimensions == 1) {
        copyElements(buffer);
        copyNulls(buffer, this->nulls, totalElements, 0);
    // If multi-dimensional, re-adjust element order
    } else {
        // Number columns
        auto cols = this->widths[1];
        // Number of elements in a row
        uint32_t rowSize = 1;
        for (uint32_t i = 2; i <= this->dimensions; i++) {
            auto *width = getFirstWidth(i);
            rowSize *= *width;
        }
        // Number of elements in a element 
        // (with more than 2 dimensions an element is a complete subarray)
        uint32_t elementSize = this->dimensions == 2 ? 1 : rowSize / this->widths[1];
        std::vector<bool> nulls;

        // Iterate over each column
        for (size_t column = 0; column < cols; column++) {
            // Iterate over each row
            for (size_t row = 0; row < totalElements; row += rowSize) {
                // Identify the value range that must be copied
                uint32_t start = row + column * elementSize;
                uint32_t stop = start + elementSize - 1;
                // Iterate over each single element
                for (size_t k = start; k <= stop; k++) {
                    if (isNull(k)) {
                        nulls.push_back(true);  
                    } else {
                        copyElement(buffer, getElementPosition(k));
                        nulls.push_back(false);
                    } 
                }
            }
        }
        copyNulls(buffer, nulls);
    }
    return toVarLen32(result);
}