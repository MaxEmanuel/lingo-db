#include "runtime/Array.h"

using runtime::Array;

runtime::VarLen32 Array::operator+(Array &other) {
    if (!isNumericType(this->type)) {
        throw std::runtime_error("Array-Add: Given element type is not numeric");
    }
    if (type != other.getType()) {
        throw std::runtime_error("Array-Add: Arrays have different types");
    }
    if (hasNullValue() || other.hasNullValue()) {
        throw std::runtime_error("Array-Add: NULL values are not allowed");
    }
    if (hasEmptyValue() || other.hasEmptyValue()) {
        throw std::runtime_error("Array-Add: Empty array elements are not allowed");
    }
    if (!equalWidths(other.getWidths())) {
        throw std::runtime_error("Array-Add: Given arrays have different structures");
    }

    std::string result;
    auto size = getStringSize(this->dimensions, this->size, getWidthSize(), getNullBytes(this->size), 0, type);
    result.resize(size);
    char *buffer = result.data();

    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &this->type, 1);
    writeToBuffer(buffer, &this->dimensions, 1);
    writeToBuffer(buffer, &this->size, 1);
    writeToBuffer(buffer, this->indices, this->dimensions);
    writeToBuffer(buffer, this->dimensionWidthMap, this->dimensions);
    writeToBuffer(buffer, this->widths, getWidthSize());

    executeBinaryOperation<ArrayAddOperator>(this->elements, other.getElements(), this->size, buffer, false, false, type);

    copyNulls(buffer, this->nulls, this->size, 0);

    return toVarLen32(result);
}

runtime::VarLen32 Array::operator-(Array &other) {
    if (!isNumericType(this->type)) {
        throw std::runtime_error("Array-Sub: Given element type is not numeric");
    }
    if (type != other.getType()) {
        throw std::runtime_error("Array-Sub: Arrays have different types");
    }
    if (hasNullValue() || other.hasNullValue()) {
        throw std::runtime_error("Array-Sub: NULL values are not allowed");
    }
    if (hasEmptyValue() || other.hasEmptyValue()) {
        throw std::runtime_error("Array-Sub: Empty array elements are not allowed");
    }
    if (!equalWidths(other.getWidths())) {
        throw std::runtime_error("Array-Sub: Given arrays have different structures");
    }

    std::string result;
    auto size = getStringSize(this->dimensions, this->size, getWidthSize(), getNullBytes(this->size), 0, type);
    result.resize(size);
    char *buffer = result.data();

    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &this->type, 1);
    writeToBuffer(buffer, &this->dimensions, 1);
    writeToBuffer(buffer, &this->size, 1);
    writeToBuffer(buffer, this->indices, this->dimensions);
    writeToBuffer(buffer, this->dimensionWidthMap, this->dimensions);
    writeToBuffer(buffer, this->widths, getWidthSize());

    executeBinaryOperation<ArraySubOperator>(this->elements, other.getElements(), this->size, buffer, false, false, type);

    copyNulls(buffer, this->nulls, this->size, 0);

    return toVarLen32(result);
}

runtime::VarLen32 Array::operator*(Array &other) {
    if (!isNumericType(this->type)) {
        throw std::runtime_error("Array-Mul: Given element type is not numeric");
    }
    if (type != other.getType()) {
        throw std::runtime_error("Array-Mul: Arrays have different types");
    }
    if (hasNullValue() || other.hasNullValue()) {
        throw std::runtime_error("Array-Mul: NULL values are not allowed");
    }
    if (hasEmptyValue() || other.hasEmptyValue()) {
        throw std::runtime_error("Array-Mul: Empty array elements are not allowed");
    }
    if (!equalWidths(other.getWidths())) {
        throw std::runtime_error("Array-Mul: Given arrays have different structures");
    }

    std::string result;
    auto size = getStringSize(this->dimensions, this->size, getWidthSize(), getNullBytes(this->size), 0, type);
    result.resize(size);
    char *buffer = result.data();

    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &this->type, 1);
    writeToBuffer(buffer, &this->dimensions, 1);
    writeToBuffer(buffer, &this->size, 1);
    writeToBuffer(buffer, this->indices, this->dimensions);
    writeToBuffer(buffer, this->dimensionWidthMap, this->dimensions);
    writeToBuffer(buffer, this->widths, getWidthSize());

    executeBinaryOperation<ArrayMulOperator>(this->elements, other.getElements(), this->size, buffer, false, false, type);

    copyNulls(buffer, this->nulls, this->size, 0);

    return toVarLen32(result);
}

runtime::VarLen32 Array::operator/(Array &other) {
    if (!isNumericType(this->type)) {
        throw std::runtime_error("Array-Div: Given element type is not numeric");
    }
    if (type != other.getType()) {
        throw std::runtime_error("Array-Div: Arrays have different types");
    }
    if (hasNullValue() || other.hasNullValue()) {
        throw std::runtime_error("Array-Div: NULL values are not allowed");
    }
    if (hasEmptyValue() || other.hasEmptyValue()) {
        throw std::runtime_error("Array-Div: Empty array elements are not allowed");
    }
    if (!equalWidths(other.getWidths())) {
        throw std::runtime_error("Array-Div: Given arrays have different structures");
    }

    std::string result;
    auto size = getStringSize(this->dimensions, this->size, getWidthSize(), getNullBytes(this->size), 0, type);
    result.resize(size);
    char *buffer = result.data();

    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &this->type, 1);
    writeToBuffer(buffer, &this->dimensions, 1);
    writeToBuffer(buffer, &this->size, 1);
    writeToBuffer(buffer, this->indices, this->dimensions);
    writeToBuffer(buffer, this->dimensionWidthMap, this->dimensions);
    writeToBuffer(buffer, this->widths, getWidthSize());

    executeBinaryOperation<ArrayDivOperator>(this->elements, other.getElements(), this->size, buffer, false, false, type);

    copyNulls(buffer, this->nulls, this->size, 0);

    return toVarLen32(result);
}

template<>
runtime::VarLen32 Array::scalarAdd(int32_t value) {
    if (type != ArrayType::INTEGER32) {
        throw std::runtime_error("Array-Add: Array elements are not of type integer (32-bit)");
    }
    return executeScalarOperation<int32_t, ArrayAddOperator>(value, true);
}

template<>
runtime::VarLen32 Array::scalarAdd(int64_t value) {
    if (type != ArrayType::INTEGER64) {
        throw std::runtime_error("Array-Add: Array elements are not of type integer (64-bit)");
    }
    return executeScalarOperation<int64_t, ArrayAddOperator>(value, true);
}

template<>
runtime::VarLen32 Array::scalarAdd(float value) {
    if (type != ArrayType::FLOAT) {
        throw std::runtime_error("Array-Add: Array elements are not of type float");
    }
    return executeScalarOperation<float, ArrayAddOperator>(value, true);
}

template<>
runtime::VarLen32 Array::scalarAdd(double value) {
    if (type != ArrayType::DOUBLE) {
        throw std::runtime_error("Array-Add: Array elements are not of type double");
    }
    return executeScalarOperation<double, ArrayAddOperator>(value, true);
}

template<>
runtime::VarLen32 Array::scalarSub(int32_t value, bool isLeft) {
    if (type != ArrayType::INTEGER32) {
        throw std::runtime_error("Array-Add: Array elements are not of type integer (32-bit)");
    }
    return executeScalarOperation<int32_t, ArraySubOperator>(value, isLeft);
}

template<>
runtime::VarLen32 Array::scalarSub(int64_t value, bool isLeft) {
    if (type != ArrayType::INTEGER64) {
        throw std::runtime_error("Array-Add: Array elements are not of type integer (64-bit)");
    }
    return executeScalarOperation<int64_t, ArraySubOperator>(value, isLeft);
}

template<>
runtime::VarLen32 Array::scalarSub(float value, bool isLeft) {
    if (type != ArrayType::FLOAT) {
        throw std::runtime_error("Array-Add: Array elements are not of type float");
    }
    return executeScalarOperation<float, ArraySubOperator>(value, isLeft);
}

template<>
runtime::VarLen32 Array::scalarSub(double value, bool isLeft) {
    if (type != ArrayType::DOUBLE) {
        throw std::runtime_error("Array-Add: Array elements are not of type double");
    }
    return executeScalarOperation<double, ArraySubOperator>(value, isLeft);
}

template<>
runtime::VarLen32 Array::scalarMul(int32_t value) {
    if (type != ArrayType::INTEGER32) {
        throw std::runtime_error("Array-Add: Array elements are not of type integer (32-bit)");
    }
    return executeScalarOperation<int32_t, ArrayMulOperator>(value, true);
}

template<>
runtime::VarLen32 Array::scalarMul(int64_t value) {
    if (type != ArrayType::INTEGER64) {
        throw std::runtime_error("Array-Add: Array elements are not of type integer (64-bit)");
    }
    return executeScalarOperation<int64_t, ArrayMulOperator>(value, true);
}

template<>
runtime::VarLen32 Array::scalarMul(float value) {
    if (type != ArrayType::FLOAT) {
        throw std::runtime_error("Array-Add: Array elements are not of type float");
    }
    return executeScalarOperation<float, ArrayMulOperator>(value, true);
}

template<>
runtime::VarLen32 Array::scalarMul(double value) {
    if (type != ArrayType::DOUBLE) {
        throw std::runtime_error("Array-Add: Array elements are not of type double");
    }
    return executeScalarOperation<double, ArrayMulOperator>(value, true);
}

template<>
runtime::VarLen32 Array::scalarDiv(int32_t value, bool isLeft) {
    if (type != ArrayType::INTEGER32) {
        throw std::runtime_error("Array-Add: Array elements are not of type integer (32-bit)");
    }
    return executeScalarOperation<int32_t, ArrayDivOperator>(value, isLeft);
}

template<>
runtime::VarLen32 Array::scalarDiv(int64_t value, bool isLeft) {
    if (type != ArrayType::INTEGER64) {
        throw std::runtime_error("Array-Add: Array elements are not of type integer (64-bit)");
    }
    return executeScalarOperation<int64_t, ArrayDivOperator>(value, isLeft);
}

template<>
runtime::VarLen32 Array::scalarDiv(float value, bool isLeft) {
    if (type != ArrayType::FLOAT) {
        throw std::runtime_error("Array-Add: Array elements are not of type float");
    }
    return executeScalarOperation<float, ArrayDivOperator>(value, isLeft);
}

template<>
runtime::VarLen32 Array::scalarDiv(double value, bool isLeft) {
    if (type != ArrayType::DOUBLE) {
        throw std::runtime_error("Array-Add: Array elements are not of type double");
    }
    return executeScalarOperation<double, ArrayDivOperator>(value, isLeft);
}

runtime::VarLen32 Array::matrixMul(Array &other) {
    if (!isFloatingPointType(this->type)) {
        throw std::runtime_error("Array-MatrixMul: Given element type must be a floating point type");
    }
    if (this->type != other.getType()) {
        throw std::runtime_error("Array-MatrixMul: Arrays have different types");
    }
    if (hasNullValue() || other.hasNullValue()) {
        throw std::runtime_error("Array-MatrixMul: NULL values are not allowed");
    }
    if (hasEmptyValue() || other.hasEmptyValue()) {
        throw std::runtime_error("Array-MatrixMul: Empty array elements are not allowed");
    }
    if (!isSymmetric() || !other.isSymmetric()) {
        throw std::runtime_error("Array-MatrixMul: This function allows only symmetric arrays");
    }
    auto otherDimension = other.getDimension();
    auto *otherWidths = other.getWidths();
    if (this->dimensions > 2 || otherDimension > 2) {
        throw std::runtime_error("Array-MatrixMul: This function allows only up to 2 dimensional arrays");
    }

    auto rowsA = this->widths[0];
    auto colsA = this->dimensions == 1 ? 1 : this->widths[1];
    auto rowsB = otherWidths[0];
    auto colsB = otherDimension == 1 ? 1 : otherWidths[1];

    if (colsA != rowsB) {
        throw std::runtime_error("Array-MatrixMul: Array-structures are not compatible for this function");
    }

    uint32_t dimension = 2;
    uint32_t elements = rowsA * colsB;

    std::string result;
    auto size = getStringSize(dimension, elements, rowsA+1, getNullBytes(elements), 0, type);
    result.resize(size);
    char *buffer = result.data();

    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.length());
    writeToBuffer(buffer, &this->type, 1);
    writeToBuffer(buffer, &dimension, 1);
    writeToBuffer(buffer, &elements, 1);
    uint32_t index = 1;
    for (uint32_t i = 0; i < dimension; i++) {
        writeToBuffer(buffer, &index, 1);
    }
    writeToBuffer(buffer, &index, 1);
    writeToBuffer(buffer, &rowsA, 1);

    writeToBuffer(buffer, &rowsA, 1);
    for (uint32_t i = 0; i < rowsA; i++) {
        writeToBuffer(buffer, &colsB, 1);
    }

    if (this->type == ArrayType::FLOAT) {
        auto *leftVal = reinterpret_cast<const float*>(this->elements);
        auto *rightVal = reinterpret_cast<const float*>(other.getElements());
        MatrixMultiplicationOperator::Operator(leftVal, rightVal, rowsA, rowsB, colsB, buffer);
    } else {
        auto *leftVal = reinterpret_cast<const double*>(this->elements);
        auto *rightVal = reinterpret_cast<const double*>(other.getElements());
        MatrixMultiplicationOperator::Operator(leftVal, rightVal, rowsA, rowsB, colsB, buffer);
    }

    return toVarLen32(result);
}