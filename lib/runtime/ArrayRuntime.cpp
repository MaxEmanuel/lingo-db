#include "runtime/ArrayRuntime.h"

using runtime::ArrayRuntime;

runtime::VarLen32 ArrayRuntime::fromString(runtime::VarLen32 str, int32_t type) {
    std::string content = str.str();
    return runtime::Array::fromString(content, type);
}

runtime::VarLen32 ArrayRuntime::getEmtpyArray(int32_t type) {
    return runtime::Array::createEmptyArray(type);
}

runtime::VarLen32 ArrayRuntime::appendArray(
    runtime::VarLen32 left,
    runtime::VarLen32 right,
    int32_t leftType,
    int32_t rightType) {
        std::string leftVal = left.str();
        std::string rightVal = right.str();
        Array leftArray(leftVal, leftType);
        Array rightArray(rightVal, rightType);
        return leftArray.append(rightArray);
}

runtime::VarLen32 ArrayRuntime::appendInt32(runtime::VarLen32 array, int32_t type, int32_t value, bool isFront) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    if (isFront) return arrayObj.appendFront(value);
    else return arrayObj.append(value);
}

runtime::VarLen32 ArrayRuntime::appendInt64(runtime::VarLen32 array, int32_t type, int64_t value, bool isFront) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    if (isFront) return arrayObj.appendFront(value);
    else return arrayObj.append(value);
}

runtime::VarLen32 ArrayRuntime::appendFloat(runtime::VarLen32 array, int32_t type, float value, bool isFront) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    if (isFront) return arrayObj.appendFront(value);
    else return arrayObj.append(value);
}

runtime::VarLen32 ArrayRuntime::appendDouble(runtime::VarLen32 array, int32_t type, double value, bool isFront) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    if (isFront) return arrayObj.appendFront(value);
    else return arrayObj.append(value);
}

runtime::VarLen32 ArrayRuntime::appendString(runtime::VarLen32 array, int32_t type, VarLen32 value, bool isFront) {
    std::string arrayVal = array.str();
    std::string valueVal = value.str();
    Array arrayObj(arrayVal, type);
    if (isFront) return arrayObj.appendFront(valueVal);
    else return arrayObj.append(valueVal);
}

runtime::VarLen32 ArrayRuntime::appendNull(runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.append();
}

runtime::VarLen32 ArrayRuntime::slice(runtime::VarLen32 array, int32_t type, int32_t lowerBound, int32_t upperBound, int32_t dimension) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.slice(lowerBound, upperBound, dimension);
}

runtime::VarLen32 ArrayRuntime::subscript(runtime::VarLen32 array, int32_t type, int32_t position) {
    std::string arrayVal = array.str();
    std::string header(arrayVal.data(), Array::ARRAYHEADER.size());
    if (header != Array::ARRAYHEADER) {
        std::string result = "";
        return Array::toVarLen32(result);
    }
    Array arrayObj(arrayVal, type);
    return arrayObj[position];
}

runtime::VarLen32 ArrayRuntime::add(
    runtime::VarLen32 left,
    runtime::VarLen32 right,
    int32_t leftType,
    int32_t rightType) {
        std::string leftVal = left.str();
        std::string rightVal = right.str();
        Array leftArray(leftVal, leftType);
        Array rightArray(rightVal, rightType);
        return leftArray + rightArray;
}

runtime::VarLen32 ArrayRuntime::sub(
    runtime::VarLen32 left,
    runtime::VarLen32 right,
    int32_t leftType,
    int32_t rightType) {
        std::string leftVal = left.str();
        std::string rightVal = right.str();
        Array leftArray(leftVal, leftType);
        Array rightArray(rightVal, rightType);
        return leftArray - rightArray;
}

runtime::VarLen32 ArrayRuntime::mul(
    runtime::VarLen32 left,
    runtime::VarLen32 right,
    int32_t leftType,
    int32_t rightType) {
        std::string leftVal = left.str();
        std::string rightVal = right.str();
        Array leftArray(leftVal, leftType);
        Array rightArray(rightVal, rightType);
        return leftArray * rightArray;
}

runtime::VarLen32 ArrayRuntime::div(
    runtime::VarLen32 left,
    runtime::VarLen32 right,
    int32_t leftType,
    int32_t rightType) {
        std::string leftVal = left.str();
        std::string rightVal = right.str();
        Array leftArray(leftVal, leftType);
        Array rightArray(rightVal, rightType);
        return leftArray / rightArray;
}

runtime::VarLen32 ArrayRuntime::scalarAddInt32(runtime::VarLen32 array, int32_t type, int32_t value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarAdd(value);
}

runtime::VarLen32 ArrayRuntime::scalarAddInt64(runtime::VarLen32 array, int32_t type, int64_t value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarAdd(value);
}

runtime::VarLen32 ArrayRuntime::scalarAddFloat(runtime::VarLen32 array, int32_t type, float value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarAdd(value);
}

runtime::VarLen32 ArrayRuntime::scalarAddDouble(runtime::VarLen32 array, int32_t type, double value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarAdd(value);
}

runtime::VarLen32 ArrayRuntime::scalarSubInt32(runtime::VarLen32 array, int32_t type, int32_t value, bool isleft) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarSub(value, isleft);
}

runtime::VarLen32 ArrayRuntime::scalarSubInt64(runtime::VarLen32 array, int32_t type, int64_t value, bool isleft) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarSub(value, isleft);
}

runtime::VarLen32 ArrayRuntime::scalarSubFloat(runtime::VarLen32 array, int32_t type, float value, bool isleft) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarSub(value, isleft);
}

runtime::VarLen32 ArrayRuntime::scalarSubDouble(runtime::VarLen32 array, int32_t type, double value, bool isleft) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarSub(value, isleft);
}

runtime::VarLen32 ArrayRuntime::scalarMulInt32(runtime::VarLen32 array, int32_t type, int32_t value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarMul(value);
}

runtime::VarLen32 ArrayRuntime::scalarMulInt64(runtime::VarLen32 array, int32_t type, int64_t value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarMul(value);
}

runtime::VarLen32 ArrayRuntime::scalarMulFloat(runtime::VarLen32 array, int32_t type, float value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarMul(value);
}

runtime::VarLen32 ArrayRuntime::scalarMulDouble(runtime::VarLen32 array, int32_t type, double value) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarMul(value);
}

runtime::VarLen32 ArrayRuntime::scalarDivInt32(runtime::VarLen32 array, int32_t type, int32_t value, bool isleft) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarDiv(value, isleft);
}

runtime::VarLen32 ArrayRuntime::scalarDivInt64(runtime::VarLen32 array, int32_t type, int64_t value, bool isleft) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarDiv(value, isleft);
}

runtime::VarLen32 ArrayRuntime::scalarDivFloat(runtime::VarLen32 array, int32_t type, float value, bool isleft) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarDiv(value, isleft);
}

runtime::VarLen32 ArrayRuntime::scalarDivDouble(runtime::VarLen32 array, int32_t type, double value, bool isleft) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.scalarDiv(value, isleft);
}

runtime::VarLen32 ArrayRuntime::fillInt32(int32_t value, runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return Array::fill(value, arrayObj);
}

runtime::VarLen32 ArrayRuntime::fillInt64(int64_t value, runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return Array::fill(value, arrayObj);
}

runtime::VarLen32 ArrayRuntime::fillFloat(float value, runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return Array::fill(value, arrayObj);
}

runtime::VarLen32 ArrayRuntime::fillDouble(double value, runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return Array::fill(value, arrayObj);
}

runtime::VarLen32 ArrayRuntime::fillString(VarLen32 value, runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    std::string val = value.str();
    Array arrayObj(arrayVal, type);
    return Array::fill(val, arrayObj);
}

runtime::VarLen32 ArrayRuntime::fillNull(runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return Array::fill(arrayObj);
}

runtime::VarLen32 ArrayRuntime::transpose(runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.transpose();
}

runtime::VarLen32 ArrayRuntime::sigmoid(runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.sigmoid();
}

runtime::VarLen32 ArrayRuntime::matrixMul(
    runtime::VarLen32 left,
    runtime::VarLen32 right,
    int32_t leftType,
    int32_t rightType) {
        std::string leftVal = left.str();
        std::string rightVal = right.str();
        Array leftArray(leftVal, leftType);
        Array rightArray(rightVal, rightType);
        return leftArray.matrixMul(rightArray);
}

runtime::VarLen32 ArrayRuntime::sum(
    runtime::VarLen32 left,
    runtime::VarLen32 right,
    int32_t leftType,
    int32_t rightType) {
        std::string leftVal = left.str();
        std::string rightVal = right.str();
        // Currently NULL values are pushed to the function
        // Treat like empty arrays
        if (leftVal == "") {
            return right;
        }
        if (rightVal == "") {
            return left;
        }
        Array leftArray(leftVal, leftType);
        Array rightArray(rightVal, rightType);
        if (leftArray.isEmpty()) {
            return right;
        }
        if (rightArray.isEmpty()) {
            return left;
        }
        return leftArray + rightArray;
}

runtime::VarLen32 ArrayRuntime::agg(
    runtime::VarLen32 left,
    runtime::VarLen32 right,
    int32_t leftType,
    int32_t rightType) {
        std::string leftVal = left.str();
        std::string rightVal = right.str();
        Array leftArray(leftVal, leftType);
        Array rightArray(rightVal, rightType);
        // If right has more dimensions
        if (rightArray.getDimension() > leftArray.getDimension()) {
            throw std::runtime_error("Array-agg: Aggregation not possible, due to inconsistent dimension structures");
        }
        // Case if state will be set to the first not empty argument
        if (leftArray.getDimension() == rightArray.getDimension()) {
            if (leftArray.isEmpty()) {
                VarLen32 result = leftArray.append(rightArray);
                leftVal = result.str();
                leftArray = Array(leftVal);
                return leftArray.increment();
            }
            left = leftArray.increment();
            leftVal = left.str();
            leftArray = Array(leftVal, leftType);
            return leftArray.append(rightArray);
        }
        return leftArray.append(rightArray);
}

int32_t ArrayRuntime::getHighestPosition(runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.getHighestPosition();
}

runtime::VarLen32 ArrayRuntime::cast(runtime::VarLen32 array, int32_t srcType, int32_t dstType) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, srcType);
    return arrayObj.cast(dstType);
}

int32_t ArrayRuntime::toInt32(runtime::VarLen32 array) {
    try {
        return std::stoi(array.str());
    } catch (std::invalid_argument &exc) {
        throw std::runtime_error(array.str() + " is not of type INTEGER");
    } catch (std::out_of_range &exc) {
        throw std::runtime_error(array.str() + " is out of range of 32-Bit INTEGER");
    }
}

int64_t ArrayRuntime::toInt64(runtime::VarLen32 array) {
    try {
        return std::stol(array.str());
    } catch (std::invalid_argument &exc) {
        throw std::runtime_error(array.str() + " is not of type INTEGER");
    } catch (std::out_of_range &exc) {
        throw std::runtime_error(array.str() + " is out of range of 64-Bit INTEGER");
    }
}

float ArrayRuntime::toFloat(runtime::VarLen32 array) {
    try {
        return std::stof(array.str());
    } catch (std::invalid_argument &exc) {
        throw std::runtime_error(array.str() + " is not of type FLOAT");
    } catch (std::out_of_range &exc) {
        throw std::runtime_error(array.str() + " is out of range of FLOAT");
    }
}

double ArrayRuntime::toDouble(runtime::VarLen32 array) {
    try {
        return std::stod(array.str());
    } catch (std::invalid_argument &exc) {
        throw std::runtime_error(array.str() + " is not of type DOUBLE");
    } catch (std::out_of_range &exc) {
        throw std::runtime_error(array.str() + " is out of range of DOUBLE");
    }
}

runtime::VarLen32 ArrayRuntime::toString(runtime::VarLen32 array) {
    std::string value = array.str();
    std::string header(value.data(), Array::ARRAYHEADER.size());
    if (header == Array::ARRAYHEADER) {
        throw std::runtime_error("Array cannot be casted to a string");
    }
    return array;
}

runtime::VarLen32 ArrayRuntime::increment(runtime::VarLen32 array, int32_t type) {
    std::string arrayVal = array.str();
    Array arrayObj(arrayVal, type);
    return arrayObj.increment();
}

bool ArrayRuntime::isNull(runtime::VarLen32 array) {
    std::string value = array.str();
    return value == "";
}

bool ArrayRuntime::equal(runtime::VarLen32 left, runtime::VarLen32 right) {
    return left.str() == right.str();
}