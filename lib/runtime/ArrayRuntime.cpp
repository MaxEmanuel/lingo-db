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

runtime::VarLen32 ArrayRuntime::appendBFloat(runtime::VarLen32 array, int32_t type, __bf16 value, bool isFront) {
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

runtime::VarLen32 ArrayRuntime::scalarAddBFloat(runtime::VarLen32 array, int32_t type, __bf16 value) {
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

runtime::VarLen32 ArrayRuntime::scalarSubBFloat(runtime::VarLen32 array, int32_t type, __bf16 value, bool isleft) {
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

runtime::VarLen32 ArrayRuntime::scalarMulBFloat(runtime::VarLen32 array, int32_t type, __bf16 value) {
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

runtime::VarLen32 ArrayRuntime::scalarDivBFloat(runtime::VarLen32 array, int32_t type, __bf16 value, bool isleft) {
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

runtime::VarLen32 ArrayRuntime::fillBFloat(__bf16 value, runtime::VarLen32 array, int32_t type) {
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

__bf16 ArrayRuntime::toBFloat(runtime::VarLen32 array) {
    try {
        float value = std::stof(array.str());
        return static_cast<__bf16>(value);
    } catch (std::invalid_argument &exc) {
        throw std::runtime_error(array.str() + " is not of type FLOAT");
    } catch (std::out_of_range &exc) {
        throw std::runtime_error(array.str() + " is out of range of FLOAT");
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

// AUTO-DIFF
/* runtime::VarLen32 runtime::ArrayRuntime::transpose(runtime::VarLen32 str, uint64_t dim, runtime::VarLen32 type) {
    if (type.str() == "int32[]") {
        runtime::Array<int32_t> array(str, dim, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        array.transpose();
        return  array.toString();
    }  else if (type.str() == "int64[]") {
        runtime::Array<int64_t> array(str, dim, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        array.transpose();
        return  array.toString();;
    } else if (type.str() == "float[]") {
        runtime::Array<float> array(str, dim, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        array.transpose();
        return  array.toString();
    } else if (type.str() == "double[]") {
        runtime::Array<double> array(str, dim, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        array.transpose();
        return  array.toString();
    } else if (type.str() == "string[]") {
        runtime::Array<std::string> array(str, dim, &runtime::TypeCasts::stringToString, &runtime::TypeCasts::stringToString);
        array.transpose();
        return  array.toString();
    } else {
        throw std::runtime_error("The entered type - " + type.str() + " - is currently not supported");
    }
    return str;
}

int32_t runtime::ArrayRuntime::castToInt32(runtime::VarLen32 str) {
    try{
        return runtime::TypeCasts::stringToInt32(str);
    } catch (const std::invalid_argument& exception) {
        std::runtime_error("The given array structure-" + str.str() + "- cannot be converted to a single integer");
    }
    return 0;
}

int64_t runtime::ArrayRuntime::castToInt64(runtime::VarLen32 str) {
    try{
        return runtime::TypeCasts::stringToInt64(str);
    } catch (const std::invalid_argument& exception) {
        std::runtime_error("The given array structure-" + str.str() + "- cannot be converted to a single integer");
    }
    return 0;
}

float runtime::ArrayRuntime::castToFloat(runtime::VarLen32 str) {
    try{
        return runtime::TypeCasts::stringToFloat(str);
    } catch (const std::invalid_argument& exception) {
        std::runtime_error("The given array structure-" + str.str() + "- cannot be converted to a single float");
    }
    return 0;
}

double runtime::ArrayRuntime::castToDouble(runtime::VarLen32 str) {
    try{
        return runtime::TypeCasts::stringToDouble(str);
    } catch (const std::invalid_argument& exception) {
        std::runtime_error("The given array structure-" + str.str() + "- cannot be converted to a single double");
    }
    return 0;
}

runtime::VarLen32 runtime::ArrayRuntime::arrayToArray(runtime::VarLen32 str, uint64_t dimensions, runtime::VarLen32 type) {
   if (type.str() == "int32[]") {
      runtime::Array<int32_t> array(str, dimensions, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
      return array.toString();
   } else if (type.str() == "int64[]") {
      runtime::Array<int64_t> array(str, dimensions, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
      return array.toString();
   } else if (type.str() == "float[]") {
      runtime::Array<float> array(str, dimensions, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
      return array.toString();
   } else if (type.str() == "double[]") {
      runtime::Array<double> array(str, dimensions, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
      return array.toString();
   } else if (type.str() == "string[]") {
      runtime::Array<std::string> array(str, dimensions, &runtime::TypeCasts::stringToString, &runtime::TypeCasts::stringToString);
      return array.toString();
   } else {
      throw std::runtime_error("The entered type - " + type.str() + " - is currently not supported");
   }
   return str;
}

runtime::VarLen32 runtime::ArrayRuntime::int32ToArray(int32_t value, uint64_t dim) {
    return runtime::TypeCasts::numericToArrayVarLen(value, dim);
}

runtime::VarLen32 runtime::ArrayRuntime::int64ToArray(int64_t value, uint64_t dim) {
    return runtime::TypeCasts::numericToArrayVarLen(value, dim);
}

runtime::VarLen32 runtime::ArrayRuntime::floatToArray(float value, uint64_t dim) {
    return runtime::TypeCasts::numericToArrayVarLen(value, dim);
}

runtime::VarLen32 runtime::ArrayRuntime::doubleToArray(double value, uint64_t dim) {
    return runtime::TypeCasts::numericToArrayVarLen(value, dim);
}

runtime::VarLen32 runtime::ArrayRuntime::nullToArray(uint64_t dim) {
    std::string result = "null";
    result = std::string(dim, '{') + result + std::string(dim, '}');
    char* array = new char[result.length()];           
    memcpy(array, result.data(), result.length());     
    return runtime::VarLen32((uint8_t*) array, result.length());
}

runtime::VarLen32 runtime::ArrayRuntime::fillLike(runtime::VarLen32 str, uint64_t dim, double value, runtime::VarLen32 valueType) {
    runtime::Array<int64_t> arrayData(str, dim, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
    std::string arrayDim = arrayData.getArray().getDimensionRange();
    std::string formattedArrayDim = "{";
    bool copyCharacter = false;
    for(size_t i = 0; i < arrayDim.size(); ++i) {
        char currentChar = arrayDim[i];
        if(currentChar == ':') {
            copyCharacter = true;
            continue;
        }
        if(currentChar == ']') {
            copyCharacter = false;
            if ((i + 1) < arrayDim.size() && arrayDim[i + 1] == '[') {
                formattedArrayDim.append(", ");
            } 
            continue;
        }
        if(copyCharacter) {
            formattedArrayDim.push_back(currentChar);
        }
    }
    formattedArrayDim.append("}");

    runtime::Array<int64_t> reconstructedArrayData(formattedArrayDim, 1, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);

    if (valueType.str() == "int32") {
        runtime::Array<int32_t> result("{}", 1, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        result.fill(reconstructedArrayData, std::to_string(value), &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        return result.toString();
    } else if (valueType.str() == "int64") {
        runtime::Array<int64_t> result("{}", 1, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        result.fill(reconstructedArrayData, std::to_string(value), &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        return result.toString();
    } else if (valueType.str() == "float") {
        runtime::Array<float> result("{}", 1, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        result.fill(reconstructedArrayData, std::to_string(value), &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        return result.toString();
    } else if (valueType.str() == "double") {
        runtime::Array<double> result("{}", 1, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        result.fill(reconstructedArrayData, std::to_string(value), &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        return result.toString();
    } else {
        throw std::runtime_error("The entered value is not supported to be used for array_fill_like");
    }

runtime::VarLen32 runtime::ArrayRuntime::matrixMul(runtime::VarLen32 str1, uint64_t dim1, runtime::VarLen32 type1, runtime::VarLen32 str2, uint64_t dim2, runtime::VarLen32 type2) {
    if (type1.str() != type2.str()) {
        throw std::runtime_error("Both arrays should have the same type - Type-1: " + type1.str() + "; Type-2: " + type2.str());
    }

    if (type1.str() == "int32[]") {
        runtime::Array<int32_t> array1(str1, dim1, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        runtime::Array<int32_t> array2(str2, dim2, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        return array2.matrixMul(array1);
    }  else if (type1.str() == "int64[]") {
        runtime::Array<int64_t> array1(str1, dim1, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        runtime::Array<int64_t> array2(str2, dim2, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        return array2.matrixMul(array1);
    } else if (type1.str() == "float[]") {
        runtime::Array<float> array1(str1, dim1, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        runtime::Array<float> array2(str2, dim2, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        return array2.matrixMul(array1);
    } else if (type1.str() == "double[]") {
        runtime::Array<double> array1(str1, dim1, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        runtime::Array<double> array2(str2, dim2, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        return array2.matrixMul(array1);
    } else {
        throw std::runtime_error("The matrix multiplication operator is only supported for numeric arrays");
    }
} */