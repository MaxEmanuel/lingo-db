#include "runtime/Array.h"

using runtime::Array;

template<>
runtime::VarLen32 Array::fill(int32_t &value, Array &structure) {
    if (structure.getSize() == 0) {
        throw std::runtime_error("Array-Fill: Array argument should contain elements");
    }
    if (structure.getType() == ArrayType::INTEGER32) {
        return generate<int32_t, int32_t>(&value, structure, ArrayType::INTEGER32);
    } else if (structure.getType() == ArrayType::INTEGER64) {
        return generate<int32_t, int64_t>(&value, structure, ArrayType::INTEGER32);
    } else {
        throw std::runtime_error("Array-Fill: Function supports only integer arrays");
    }
}

template<>
runtime::VarLen32 Array::fill(int64_t &value, Array &structure) {
    if (structure.getSize() == 0) {
        throw std::runtime_error("Array-Fill: Array argument should contain elements");
    }
    if (structure.getType() == ArrayType::INTEGER32) {
        return generate<int64_t, int32_t>(&value, structure, ArrayType::INTEGER64);
    } else if (structure.getType() == ArrayType::INTEGER64) {
        return generate<int64_t, int64_t>(&value, structure, ArrayType::INTEGER64);
    } else {
        throw std::runtime_error("Array-Fill: Function supports only integer arrays");
    }
}

template<>
runtime::VarLen32 Array::fill(float &value, Array &structure) {
    if (structure.getSize() == 0) {
        throw std::runtime_error("Array-Fill: Array argument should contain elements");
    }
    if (structure.getType() == ArrayType::INTEGER32) {
        return generate<float, int32_t>(&value, structure, ArrayType::FLOAT);
    } else if (structure.getType() == ArrayType::INTEGER64) {
        return generate<float, int64_t>(&value, structure, ArrayType::FLOAT);
    } else {
        throw std::runtime_error("Array-Fill: Function supports only integer arrays");
    }
}

template<>
runtime::VarLen32 Array::fill(double &value, Array &structure) {
    if (structure.getSize() == 0) {
        throw std::runtime_error("Array-Fill: Array argument should contain elements");
    }
    if (structure.getType() == ArrayType::INTEGER32) {
        return generate<double, int32_t>(&value, structure, ArrayType::DOUBLE);
    } else if (structure.getType() == ArrayType::INTEGER64) {
        return generate<double, int64_t>(&value, structure, ArrayType::DOUBLE);
    } else {
        throw std::runtime_error("Array-Fill: Function supports only integer arrays");
    }
}

template<>
runtime::VarLen32 Array::fill(std::string &value, Array &structure) {
    if (structure.getSize() == 0) {
        throw std::runtime_error("Array-Fill: Array argument should contain elements");
    }
    if (structure.getType() == ArrayType::INTEGER32) {
        return generate<char, int32_t>(value.data(), structure, ArrayType::STRING, value.size());
    } else if (structure.getType() == ArrayType::INTEGER64) {
        return generate<char, int64_t>(value.data(), structure, ArrayType::STRING, value.size());
    } else {
        throw std::runtime_error("Array-Fill: Function supports only integer arrays");
    }
}

runtime::VarLen32 Array::fill(Array &structure) {
    if (structure.getSize() == 0) {
        throw std::runtime_error("Array-Fill: Array argument should contain elements");
    }
    if (structure.getType() == ArrayType::INTEGER32) {
        return generate<int32_t>(structure, ArrayType::INTEGER32);
    } else if (structure.getType() == ArrayType::INTEGER64) {
        return generate<int64_t>(structure, ArrayType::INTEGER32);
    } else {
        throw std::runtime_error("Array-Fill: Function supports only integer arrays");
    }
}