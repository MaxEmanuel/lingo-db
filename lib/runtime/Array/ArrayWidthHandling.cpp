#include "runtime/Array.h"

using runtime::Array;

uint32_t Array::getOffset(const uint32_t *width) {
    uint32_t offset = 0;
    auto *entries = getFirstWidth(this->dimensions);
    auto limit = this->dimensionWidthMap[this->dimensions-1];
    // Iterate over each width of last dimension
    for (uint32_t i = 0; i < limit; i++) {
        // Compare pointer
        if (entries + i == width) return offset;
        else offset += entries[i];
    }
    // If this happens, the width does not exist
    throw std::runtime_error("Requested width does not exist");
}

uint32_t Array::getWidthSize() {
    uint32_t result = 0;
    // Count each value from map attribute
    for (uint32_t i = 0; i < this->dimensions; i++) {
        result += this->dimensionWidthMap[i];
    }
    return result;
}

uint32_t Array::getWidthSize(uint32_t dimension) {
    if (dimension > this->dimensions || dimension == 0) {
        throw std::runtime_error("Requested dimension does not exist");
    }
    return this->dimensionWidthMap[dimension-1];
}

const uint32_t* Array::getWidths() {
    return this->widths;
}

const uint32_t *Array::getFirstWidth(uint32_t dimension) {
    if (dimension > this->dimensions || dimension == 0) {
        throw std::runtime_error("Requested dimension does not exist");
    }
    uint32_t *width = this->widths;
    for (uint32_t i = 0; i < dimension - 1; i++) {
        width += this->dimensionWidthMap[i];
    }
    return width;
}

const uint32_t *Array::getChildWidth(const uint32_t *width, uint32_t dimension) {
    if (dimension > this->dimensions || dimension == 0) {
        throw std::runtime_error("Requested dimension does not exist");
    }
    // If structure is empty, return nullptr
    if (width[0] == 0) return nullptr;
    auto *entries = getFirstWidth(dimension);
    auto length = this->dimensionWidthMap[dimension-1];
    uint32_t ignore = 0;
    // Iterate over widths of provided dimension
    for (uint32_t i = 0; i <= length; i++) {
        if (width == &entries[i]) return entries += length + ignore;
        // Count values of previous widths
        else ignore += entries[i];
    }
    return nullptr;
}

bool Array::equalWidths(const uint32_t *other) {
    auto size = getWidthSize();
    // Iterate over each width value
    for (uint32_t i = 0; i < size; i++) {
        if (this->widths[i] != other[i]) {
            return false;
        }
    }
    return true;
}

bool Array::isSymmetric() {
    // Iterate over each dimension
    for (uint32_t i = 1; i <= this->dimensions; i++) {
        auto size = getWidthSize(i);
        auto *entries = getFirstWidth(i);
        auto width = entries[0];
        // Iterate over each width entry in a particular dimension
        for (uint32_t j = 0; j < size; j++) {
            if (entries[j] != width) return false;
        }
    }
    return true;
}