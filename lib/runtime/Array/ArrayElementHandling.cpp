#include "runtime/Array.h"

using runtime::Array;

const uint8_t* Array::getElements() {
    return this->elements;
}

uint32_t Array::getSize(bool withNulls) {
    if (withNulls) {
        // With nulls iterate over widths of last dimension and count values
        auto *start = getFirstWidth(this->dimensions);
        auto limit = this->dimensionWidthMap[this->dimensions-1];
        uint32_t result = 0;
        for (uint32_t i = 0; i < limit; i++) {
            result += start[i];
        }
        return result;
    }
    return this->size;
}

uint32_t Array::getDimensionSize(uint32_t dimension) {
    if (dimension > this->dimensions || dimension == 0) {
        throw std::runtime_error("Requested array dimension does not exist");
    }
    auto length = this->dimensionWidthMap[dimension-1];
    auto *entries = getFirstWidth(dimension);
    uint32_t result = 0;
    // Iterate over each width in dimension and select largest
    for (uint32_t i = 0; i < length; i++) {
        if (entries[i] > result) result = entries[i];
    }
    return result;
}

uint32_t Array::getStringLength() {
    if (this->type != ArrayType::STRING) return 0;
    uint32_t result = 0;
    // Iterate over each string length value
    auto *lengths = reinterpret_cast<uint32_t*>(this->elements);
    for (size_t i = 0; i < this->size; i++) {
        result += lengths[i];
    }
    return result;
}

uint32_t Array::getStringLength(uint32_t position) {
    if (this->size <= position) {
        throw std::runtime_error("Requested array element does not exist");
    }
    if (this->type != ArrayType::STRING) return 0;
    uint32_t *lengths = reinterpret_cast<uint32_t*>(this->elements);
    return lengths[position];
}

int32_t Array::getHighestPosition() {
    if (this->size == 0) return 0;
    switch (this->type) {
    case ArrayType::INTEGER32:
        return getMaxIndex<int32_t>();
    case ArrayType::INTEGER64:
        return getMaxIndex<int64_t>();
    case ArrayType::FLOAT:
        return getMaxIndex<float>();
    case ArrayType::DOUBLE:
        return getMaxIndex<double>();
    case ArrayType::STRING:
        return getMaxIndex<uint32_t>();
    default:
        throw std::runtime_error("Array-HighestPosition: Given array type is not supported in arrays");
    }
}

void Array::copyElements(char *&buffer) {
    if (this->size == 0) return;
    switch (this->type) {
    case ArrayType::INTEGER32:
        writeToBuffer(buffer, reinterpret_cast<int32_t*>(this->elements), this->size);
        break;
    case ArrayType::INTEGER64:
        writeToBuffer(buffer, reinterpret_cast<int64_t*>(this->elements), this->size);
        break;
    case ArrayType::FLOAT:
        writeToBuffer(buffer, reinterpret_cast<float*>(this->elements), this->size);
        break;
    case ArrayType::DOUBLE:
        writeToBuffer(buffer, reinterpret_cast<double*>(this->elements), this->size);
        break;
    case ArrayType::STRING:
        writeToBuffer(buffer, reinterpret_cast<uint32_t*>(this->elements), this->size);
        break;
    default:
        throw std::runtime_error("Copy-Operation: Given array type is not supported in arrays");
    }
}

void Array::copyElement(char *&buffer, uint32_t position) {
    if (this->size <= position) {
        throw std::runtime_error("Requested array element does not exist");
    }
    switch (this->type) {
    case ArrayType::INTEGER32: 
    {
        int32_t *value = reinterpret_cast<int32_t*>(this->elements) + position;
        writeToBuffer(buffer, value, 1);
        break;
    }
    case ArrayType::INTEGER64: 
    {
        int64_t *value = reinterpret_cast<int64_t*>(this->elements) + position;
        writeToBuffer(buffer, value, 1);
        break;
    }
    case ArrayType::FLOAT: 
    {
        float *value = reinterpret_cast<float*>(this->elements) + position;
        writeToBuffer(buffer, value, 1);
        break;
    }
    case ArrayType::DOUBLE: 
    {
        double *value = reinterpret_cast<double*>(this->elements) + position;
        writeToBuffer(buffer, value, 1);
        break;
    }
    case ArrayType::STRING: 
    {
        uint32_t *value = reinterpret_cast<uint32_t*>(this->elements) + position;
        writeToBuffer(buffer, value, 1);
        break;
    }
    default:
        throw std::runtime_error("Copy-Operation: Given array type is not supported in arrays");
    }
}

void Array::copyStrings(char *&buffer) {
    if (type != ArrayType::STRING) return;
    size_t size = getStringLength();
    writeToBuffer(buffer, this->strings, size);
}

void Array::copyString(char *&buffer, uint32_t position) {
    if (this->size <= position) {
        throw std::runtime_error("Requested array element does not exist");
    }
    uint32_t *lengths = reinterpret_cast<uint32_t*>(this->elements);
    uint32_t offset = 0;
    // Iterate over string lengths to get position of first character
    for (size_t i = 0; i < position; i++) {
        offset += lengths[i];
    }
    writeToBuffer(buffer, this->strings + offset, lengths[position]);
}

template<>
void Array::castAndCopyElement<int32_t>(char *&buffer, std::string &value) {
    try {
        int32_t castValue = std::stoi(value);
        writeToBuffer(buffer, &castValue, 1);
    } catch (std::invalid_argument &exc) {
        throw std::runtime_error(value + " is not of type INTEGER");
    } catch (std::out_of_range &exc) {
        throw std::runtime_error(value + " is out of range of 32-Bit INTEGER");
    }
}

template<>
void Array::castAndCopyElement<int64_t>(char *&buffer, std::string &value) {
    try {
        int64_t castValue = std::stol(value);
        writeToBuffer(buffer, &castValue, 1);
    } catch (std::invalid_argument &exc) {
        throw std::runtime_error(value + " is not of type INTEGER");
    } catch (std::out_of_range &exc) {
        throw std::runtime_error(value + " is out of range of 64-Bit INTEGER");
    }
}

template<>
void Array::castAndCopyElement<float>(char *&buffer, std::string &value) {
    try {
        float castValue = std::stof(value);
        writeToBuffer(buffer, &castValue, 1);
    } catch (std::invalid_argument &exc) {
        throw std::runtime_error(value + " is not of type FLOAT");
    } catch (std::out_of_range &exc) {
        throw std::runtime_error(value + " is out of range of FLOAT");
    }
}

template<>
void Array::castAndCopyElement<double>(char *&buffer, std::string &value) {
    try {
        double castValue = std::stod(value);
        writeToBuffer(buffer, &castValue, 1);
    } catch (std::invalid_argument &exc) {
        throw std::runtime_error(value + " is not of type DOUBLE");
    } catch (std::out_of_range &exc) {
        throw std::runtime_error(value + " is out of range of DOUBLE");
    }
}

template<>
void Array::castAndCopyElement<std::string>(char *&buffer, std::string &value) {
    writeToBuffer(buffer, &value, value.size());
}

uint32_t Array::getElementPosition(uint32_t position) {
    auto size = getSize(true);
    if (size <= position) {
        throw std::runtime_error("Requested array element does not exist");
    }
    return position - countNulls(position);
}

template<>
void Array::toString<int32_t>(uint32_t position, std::string &target) {
    if (this->size <= position) {
        throw std::runtime_error("Requested array element does not exist");
    }
    int32_t value = *reinterpret_cast<int32_t*>(this->elements + position * sizeof(int32_t));
    target.append(std::to_string(value));
}

template<>
void Array::toString<int64_t>(uint32_t position, std::string &target) {
    if (this->size <= position) {
        throw std::runtime_error("Requested array element does not exist");
    }
    int64_t value = *reinterpret_cast<int64_t*>(this->elements + position * sizeof(int64_t));
    target.append(std::to_string(value));
}

template<>
void Array::toString<float>(uint32_t position, std::string &target) {
    if (this->size <= position) {
        throw std::runtime_error("Requested array element does not exist");
    }
    float value = *reinterpret_cast<float*>(this->elements + position * sizeof(float));
    target.append(std::to_string(value));
}

template<>
void Array::toString<double>(uint32_t position, std::string &target) {
    if (this->size <= position) {
        throw std::runtime_error("Requested array element does not exist");
    }
    double value = *reinterpret_cast<double*>(this->elements + position * sizeof(double));
    target.append(std::to_string(value));
}

template<>
void Array::toString<std::string>(uint32_t position, std::string &target) {
    if (this->size <= position) {
        throw std::runtime_error("Requested array element does not exist");
    }
    auto *lengths = reinterpret_cast<uint32_t*>(this->elements);
    uint32_t offset = 0;

    for (uint32_t i = 0; i < position; i++) {
        offset += lengths[i];
    }
    std::string value = std::string(this->strings + offset, lengths[position]);
    target.append("\"");
    target.append(value);
    target.append("\"");
}

bool Array::hasEmptyValue() {
    auto size = getWidthSize();
    // Iterate over each width entry
    for (uint32_t i = 0; i < size; i++) {
        if (this->widths[i] == 0) return true;
    }
    return false;
}