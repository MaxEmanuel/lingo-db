#include "runtime/Array.h"

using runtime::Array;

template<>
runtime::VarLen32 Array::appendElement(std::string value) {
    // Variables that will change in new array
    auto numberElements = this->size + 1;
    auto totalElements = getSize(true) + 1;
    auto widthSize = getWidthSize();
    auto lastWidth = this->widths[widthSize-1] + 1;
    uint32_t length = value.length();
    auto stringLengths = getStringLength() + length;

    std::string result;
    size_t size = getStringSize(this->dimensions, numberElements, widthSize, getNullBytes(totalElements), stringLengths, this->type);
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
    writeToBuffer(buffer, &length, 1);
    copyNulls(buffer, this->nulls, totalElements, 0);
    copyStrings(buffer);
    writeToBuffer(buffer, value.data(), length);
    
    return toVarLen32(result);
};

template<>
runtime::VarLen32 Array::appendElementFront(std::string value) {
    // Variables that will change in new array
    auto numberElements = this->size + 1;
    auto totalElements = getSize(true) + 1;
    auto widthSize = getWidthSize();
    auto lastWidthSize = getWidthSize(this->dimensions);
    auto changedWidth = this->widths[widthSize - lastWidthSize] + 1;
    uint32_t length = value.length();
    auto stringLengths = getStringLength() + length;

    std::string result;
    size_t size = getStringSize(this->dimensions, numberElements, widthSize, getNullBytes(totalElements), stringLengths, this->type);
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
    writeToBuffer(buffer, &length, 1);
    copyElements(buffer);
    copyNulls(buffer, this->nulls, totalElements, 0);
    writeToBuffer(buffer, value.data(), length);
    copyStrings(buffer);
    return toVarLen32(result);
}

template<>
runtime::VarLen32 Array::append(Array &toAppend) {
    if (this->type != toAppend.getType()) {
        throw std::runtime_error("Array-Append: Arrays have different types");
    }
    if (this->dimensions < toAppend.getDimension()) {
        throw std::runtime_error("Array-Append: Right array has more dimensions than left array");
    }

    auto leftDimension = this->dimensions;
    auto rightDimension = toAppend.getDimension();

    auto leftElements = getSize();
    auto leftTotalElements = getSize(true);
    auto rightElements = toAppend.getSize();
    auto rightTotalElements = toAppend.getSize(true);

    auto leftStringLength = getStringLength();
    auto rightStringLength = toAppend.getStringLength();

    auto widthSize = getWidthSize() + toAppend.getWidthSize();
    // First dimension of right will not be copied to left if both have the same number of dimensions
    if (leftDimension == rightDimension) widthSize--;

    // First step: calculate the new size of the resulting string
    auto size = getStringSize(
        leftDimension, 
        leftElements + rightElements,
        widthSize,
        getNullBytes(leftTotalElements + rightTotalElements),
        leftStringLength + rightStringLength,
        type
    );
    // result will store the extended array
    std::string result;
    result.resize(size);
    char *buffer = result.data();

    // Add identifier and type
    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &this->type, 1);

    // Add number dimensions and elements to the result
    uint32_t numberElements = leftElements + rightElements;
    writeToBuffer(buffer, &leftDimension, 1);
    writeToBuffer(buffer, &numberElements, 1);

    // Add indicies
    writeToBuffer(buffer, this->indices, leftDimension);

    // Add width lengths to the result
    uint32_t leftIdx = 1;
    uint32_t rightIdx = 1;
    // Iterate over every dimension
    for (uint32_t i = leftDimension; i > 0; i--) {
        auto size = getWidthSize(leftIdx);
        if (rightDimension == i && rightDimension == leftDimension) {
            // Left and right have same dimension. First width if right will be ignored
            writeToBuffer(buffer, &size, 1);
            rightIdx++;
        } else if (rightDimension < i) {
            // Right does not have any widths in this dimension
            writeToBuffer(buffer, &size, 1);
        } else {
            // All width entries of right will be copied to left
            size += toAppend.getWidthSize(rightIdx);
            writeToBuffer(buffer, &size, 1);
            rightIdx++;
        }
        leftIdx++;
    }

    // Add width entries to the result
    leftIdx = 1;
    rightIdx = 1;
    for (uint32_t i = leftDimension; i > 0; i--) {
        auto *leftWidths = getFirstWidth(leftIdx);
        auto *rightWidths = toAppend.getFirstWidth(rightIdx);
        auto leftWidthSize = getWidthSize(leftIdx);
        auto rightWidthSize = toAppend.getWidthSize(rightIdx);

        if (rightDimension == i && rightDimension == leftDimension) {
            // Both arrays have equal dimensions -> combine both width values
            auto newWidth = leftWidths[0] + rightWidths[0];
            writeToBuffer(buffer, &newWidth, 1);
            rightIdx++;
        } else if (rightDimension+1 == i) {
            // Left dimension is exactly one level above the right array dimension
            // Write all left width entries and modify last one
            writeToBuffer(buffer, leftWidths, leftWidthSize-1);
            auto newWidth = leftWidths[leftWidthSize-1] + 1;
            writeToBuffer(buffer, &newWidth, 1);
        } else if (rightDimension+1 < i) {
            // Dimension only exist on left array
            writeToBuffer(buffer, leftWidths, leftWidthSize);
        } else {
            // Both arrays have widths in that dimension
            writeToBuffer(buffer, leftWidths, leftWidthSize);
            writeToBuffer(buffer, rightWidths, rightWidthSize);
            rightIdx++;
        }
        leftIdx++;
    }
    // Add elements to result
    copyElements(buffer);
    toAppend.copyElements(buffer);

    // Add null values to result
    copyNulls(buffer, this->nulls, leftTotalElements, 0);
    // Step one char back if some bits in last byte not used 
    if (leftTotalElements % 8 != 0) {
        buffer -= 1;
    } 
    copyNulls(buffer, toAppend.getNulls(), rightTotalElements, leftTotalElements);

    // Add string values to result
    copyStrings(buffer);
    toAppend.copyStrings(buffer);

    return toVarLen32(result);
}

runtime::VarLen32 Array::append() {
    // Variables that will change in new array
    auto totalElements = getSize(true) + 1;
    auto nullBytes = getNullBytes(totalElements);
    auto widthSize = getWidthSize();
    auto stringLengths = getStringLength();
    auto lastWidth = this->widths[widthSize-1] + 1;

    std::string result;
    size_t size = getStringSize(this->dimensions, this->size, widthSize, nullBytes, stringLengths, this->type);
    result.resize(size);
    char *buffer = result.data();

    // Write complete content to result string
    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &this->type, 1);
    writeToBuffer(buffer, &this->dimensions, 1);
    writeToBuffer(buffer, &this->size, 1);
    writeToBuffer(buffer, this->indices, this->dimensions);
    writeToBuffer(buffer, this->dimensionWidthMap, this->dimensions);
    writeToBuffer(buffer, this->widths, widthSize-1);
    writeToBuffer(buffer, &lastWidth, 1);
    copyElements(buffer);
    if (totalElements-1 != 0) {
        writeToBuffer(buffer, this->nulls, nullBytes);
    }
    // Add new null bit
    if ((totalElements-1) % 8 != 0) {
        buffer--;
    }
    uint8_t newIndex = (totalElements-1) % 8;
    uint8_t newShift = 8 - newIndex - 1;
    *buffer |= (1 << newShift);
    buffer++;
    copyStrings(buffer);
    return toVarLen32(result);
}

template<>
runtime::VarLen32 Array::append(int32_t &toAppend) {
    if (type != ArrayType::INTEGER32) {
        throw std::runtime_error("Array-Append: Array elements are not of type integer (32-bit)");
    }
    return appendElement(toAppend);
}

template<>
runtime::VarLen32 Array::append(int64_t &toAppend) {
    if (type != ArrayType::INTEGER64) {
        throw std::runtime_error("Array-Append: Array elements are not of type integer (64-bit)");
    }
    return appendElement(toAppend);
}

template<>
runtime::VarLen32 Array::append(float &toAppend) {
    if (type != ArrayType::FLOAT) {
        throw std::runtime_error("Array-Append: Array elements are not of type float");
    }
    return appendElement(toAppend);
}

template<>
runtime::VarLen32 Array::append(double &toAppend) {
    if (type != ArrayType::DOUBLE) {
        throw std::runtime_error("Array-Append: Array elements are not of type double");
    }
    return appendElement(toAppend);
}

template<>
runtime::VarLen32 Array::append(std::string &toAppend) {
    if (type != ArrayType::STRING) {
        throw std::runtime_error("Array-Append: Array elements are not of type string");
    }
    return appendElement(toAppend);
}

template<>
runtime::VarLen32 Array::appendFront(int32_t &toAppend) {
    if (type != ArrayType::INTEGER32) {
        throw std::runtime_error("Array-Append: Array elements are not of type integer (32-bit)");
    }
    return appendElementFront(toAppend);
}

template<>
runtime::VarLen32 Array::appendFront(int64_t &toAppend) {
    if (type != ArrayType::INTEGER64) {
        throw std::runtime_error("Array-Append: Array elements are not of type integer (64-bit)");
    }
    return appendElementFront(toAppend);
}

template<>
runtime::VarLen32 Array::appendFront(float &toAppend) {
    if (type != ArrayType::FLOAT) {
        throw std::runtime_error("Array-Append: Array elements are not of type float");
    }
    return appendElementFront(toAppend);
}

template<>
runtime::VarLen32 Array::appendFront(double &toAppend) {
    if (type != ArrayType::DOUBLE) {
        throw std::runtime_error("Array-Append: Array elements are not of type double");
    }
    return appendElementFront(toAppend);
}

template<>
runtime::VarLen32 Array::appendFront(std::string &toAppend) {
    if (type != ArrayType::STRING) {
        throw std::runtime_error("Array-Append: Array elements are not of type string");
    }
    return appendElementFront(toAppend);
}