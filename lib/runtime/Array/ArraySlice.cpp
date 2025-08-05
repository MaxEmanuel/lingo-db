#include "runtime/Array.h"

using runtime::Array;

runtime::VarLen32 Array::slice(int32_t lowerBound, int32_t upperBound, uint32_t dimension) {
    // lower bound must be smaller than upper bound
    if (lowerBound > upperBound) {
        throw std::runtime_error("Array-Slice: Given lower-bound is larger than given upper-bound");
    }
    // Slice in a dimension that does not exist
    if (dimension > this->dimensions) {
        return createEmptyArray(type);
    }

    // These variables store updated array data
    std::vector<uint32_t> widths;
    std::vector<uint32_t> widthSize;
    std::vector<uint32_t> elementIdx;
    std::vector<bool> nulls;
    uint32_t numberElements = 0;
    uint32_t stringLengths = 0;

    // Size of this vector keeps the same as in the current array
    widthSize.resize(this->dimensions, 0);
    
    const auto *start = this->widths;
    getArraySlice(widths, widthSize, elementIdx, lowerBound, upperBound, dimension, 1, start);
    // Update width of first dimension
    widthSize[0] = 1;
    widths.insert(widths.begin(), widthSize[1]);
    nulls.reserve(elementIdx.size());

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

    // If slice did not include any elements, return empty array which has only one dimension
    auto dimensions = elementIdx.size() == 0 ? 1 : this->dimensions;

    // Define result string
    std::string result;
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

    // Iterate over each dimension to update dimension indices
    for (uint32_t i = 0; i < dimensions; i++) {
        if (elementIdx.size() == 0) {
            // If slice did not include any elements, return empty array which gets the default index
            uint32_t value = 1;
            writeToBuffer(buffer, &value, 1); 
        } else if (i+1 == dimension) {
            // If slice dimension reached, use the lower bound as new start index
            writeToBuffer(buffer, &lowerBound, 1);   
        } else {
            // Everything else will get its original index
            writeToBuffer(buffer, &this->indices[i], 1);
        }
    }

    writeToBuffer(buffer, widthSize.data(), dimensions);

    if (elementIdx.size() == 0) {
        // If slice did not include any elements, return empty array which gets a zero width
        uint32_t value = 0;
        writeToBuffer(buffer, &value, 1); 
    } else {
        // Otherwise copy all data from widths vector
        writeToBuffer(buffer, widths.data(), widths.size());
    }

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

void Array::getArraySlice(
    std::vector<uint32_t> &widths, 
    std::vector<uint32_t> &widthSize, 
    std::vector<uint32_t> &elements, 
    int32_t lowerBound,
    int32_t upperBound,
    uint32_t sliceDimension, 
    uint32_t dimension,
    const uint32_t *&entry) {

    auto isSlice = dimension == sliceDimension;
    auto index = this->indices[dimension-1];
    // Case for last dimension
    if (dimension == this->dimensions) {
        // Get position of first element
        auto offset = getOffset(entry);
        for (int32_t i = index; i < index + (int32_t) entry[0]; i++) {
            // If outside the interval, ignore value
            if (isSlice && (i < lowerBound || i > upperBound)) {
                offset++;
                continue;
            }
            // Otherwise add its index
            elements.push_back(offset);
            offset++;
        }
        return;
    }

    // Case for other dimensions
    auto *children = getChildWidth(entry, dimension);
    // Identify insert position for the widths vector
    auto widthPosition = 0;
    for (uint32_t i = 0; i <= dimension; i++) {
        widthPosition += widthSize[i];
    }
    // Iterate over each child width entry
    for (int32_t i = index; i < index + (int32_t) entry[0]; i++) {
        // If outside the interval, ignore structure
        if (isSlice && (i < lowerBound || i > upperBound)) {
            continue;
        }
        // Otherwise incrementing widht size, because child should remain
        *(widthSize.begin() + dimension) += 1;
        // Add new width entry
        widths.insert(widths.begin() + widthPosition, 0);
        // Store important information to later update the width of this child
        uint32_t state = dimension+1 == this->dimensions ? elements.size() : widthSize[dimension+1];
        // Check child entry recursively
        auto *child = children + (i-index);
        getArraySlice(widths, widthSize, elements, lowerBound, upperBound, sliceDimension, dimension + 1, child);
        // Calculate the correct width of this child
        if (dimension+1 == this->dimensions) {
            // By identifying how many elements have been detected after processing its childs (last dimension)
            widths[widthPosition] = elements.size() - state;
        } else {
            // By identifying how many array structures have been detected after processing its childs (other dimensions)
            widths[widthPosition] = widthSize[dimension+1] - state;
        }
        widthPosition += 1;
    }
}