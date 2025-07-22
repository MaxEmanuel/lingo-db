#include "runtime/Array.h"

using runtime::Array;

const uint8_t* Array::getNulls() {
    return this->nulls;
}

void Array::copyNulls(char *&buffer, std::vector<bool> &nulls) {
    // Iterate over each vector element
    for (size_t i = 0; i < nulls.size(); i++) {
        // Check if new byte must be selected
        if (i != 0 && i % 8 == 0) {
            buffer += 1;
        }
        if (nulls[i]) {
            // If value is true, add true bit 
            uint8_t index = i % 8;
            uint8_t shift = 8 - index - 1;
            *buffer |= (1 << shift);
        }
    }
    // Step to the next empty byte
    buffer += 1;
}

void Array::copyNulls(char *&buffer, const uint8_t *nulls, uint32_t size, uint32_t position) {
    // If no previous nulls, copy null bitstrings directly
    if (position == 0) {
        writeToBuffer(buffer, nulls, getNullBytes(size));
        return;
    }
    // Iterate over each position
    for (uint32_t i = 0; i < size; i++) {
        // Check if new null byte must be selected
        if (i != 0 && i % 8 == 0) {
            nulls++;
        }
        // Get boolean value from nulls at position i
        uint8_t index = i % 8;
        uint8_t shift = 8 - index - 1;
        bool isNull = (nulls[0] >> shift) & 1;
        if (isNull) {
            // If true, push bit to new position
            uint8_t newIndex = position % 8;
            uint8_t newShift = 8 - newIndex - 1;
            *buffer |= (1 << newShift);
        }
        // Check if new byte in result must be selected
        position++;
        if (position % 8 == 0) {
            buffer++;
        }
    }
    // Step to the next empty byte
    buffer++;
}

bool Array::isNull(uint32_t position) {
    auto size = getSize(true);
    if (size <= position) {
        throw std::runtime_error("Array-Element does not exist");
    }
    // Identify null byte
    uint32_t element = std::floor(position / 8);
    // Identify bit position in that byte
    uint32_t index = position % 8;
    uint32_t shift = 8 - index - 1;
    return 1 & (this->nulls[element] >> shift);
}

uint32_t Array::countNulls(uint32_t position) {
    // Check if position is valid
    auto size = getSize(true);
    if (size <= position) {
        throw std::runtime_error("Array-Element does not exist");
    }
    // Iterate over each position until reaching parameter value
    uint32_t result = 0;
    for (uint32_t i = 0; i < position; i++) {
        if (isNull(i)) {
            result++;
        }
    }
    return result;
}

uint32_t Array::getNullBytes(uint32_t size) {
    return std::ceil((double) size / 8);
}

bool Array::hasNullValue() {
    // Iterate over each position and check if it is null
    auto size = getSize(true);
    for (uint32_t i = 0; i < size; i++) {
        if (isNull(i)) {
            return true;
        }
    }
    return false;
}