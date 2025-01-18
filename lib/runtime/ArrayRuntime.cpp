#include "runtime/helpers.h"
#include "runtime/ArrayRuntime.h"
#include "runtime/ArrayRuntime/Array.h"
#include "runtime/ArrayRuntime/TypeCast.h"

runtime::VarLen32 runtime::ArrayRuntime::concat(runtime::VarLen32 str1, uint64_t dim1, runtime::VarLen32 type1, runtime::VarLen32 str2, uint64_t dim2, runtime::VarLen32 type2) {
    if (type1.str() != type2.str()) {
        throw std::runtime_error("Both arrays should have the same type - Type-1: " + type1.str() + "; Type-2: " + type2.str());
    }

    if (type1.str() == "int32[]") {
        runtime::Array<int32_t> array1(str1, dim1, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        runtime::Array<int32_t> array2(str2, dim2, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        array1.concat(array2);
        return array1.toString();
    }  else if (type1.str() == "int64[]") {
        runtime::Array<int64_t> array1(str1, dim1, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        runtime::Array<int64_t> array2(str2, dim2, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        array1.concat(array2);
        return array1.toString();
    } else if (type1.str() == "float[]") {
        runtime::Array<float> array1(str1, dim1, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        runtime::Array<float> array2(str2, dim2, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        array1.concat(array2);
        return array1.toString();
    } else if (type1.str() == "double[]") {
        runtime::Array<double> array1(str1, dim1, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        runtime::Array<double> array2(str2, dim2, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        array1.concat(array2);
        return array1.toString();
    } else if (type1.str() == "string[]") {
        runtime::Array<std::string> array1(str1, dim1, &runtime::TypeCasts::stringToString, &runtime::TypeCasts::stringToString);
        runtime::Array<std::string> array2(str2, dim2, &runtime::TypeCasts::stringToString, &runtime::TypeCasts::stringToString);
        array1.concat(array2);
        return array1.toString();
    } else {
        throw std::runtime_error("The entered type - " + type1.str() + " - is currently not supported");
    }
}

runtime::VarLen32 runtime::ArrayRuntime::getRange(runtime::VarLen32 str, uint64_t dim, runtime::VarLen32 type, uint64_t start, uint64_t stop, uint64_t subDim) {
    if (type.str() == "int32[]") {
        runtime::Array<int32_t> array(str, dim, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        array.slice(start, stop, subDim);
        return  array.toString();
    }  else if (type.str() == "int64[]") {
        runtime::Array<int64_t> array(str, dim, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        array.slice(start, stop, subDim);
        return  array.toString();
    } else if (type.str() == "float[]") {
        runtime::Array<float> array(str, dim, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        array.slice(start, stop, subDim);
        return  array.toString();
    } else if (type.str() == "double[]") {
        runtime::Array<double> array(str, dim, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        array.slice(start, stop, subDim);
        return  array.toString();
    } else if (type.str() == "string[]") {
        runtime::Array<std::string> array(str, dim, &runtime::TypeCasts::stringToString, &runtime::TypeCasts::stringToString);
        array.slice(start, stop, subDim);
        return  array.toString();
    } else {
        throw std::runtime_error("The entered type - " + type.str() + " - is currently not supported");
    }
    return str;
}

runtime::VarLen32 runtime::ArrayRuntime::getEntry(runtime::VarLen32 str, uint64_t dim, runtime::VarLen32 type, uint64_t index) {
    if (type.str() == "int32[]") {
        runtime::Array<int32_t> array(str, dim, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        return  array.getEntry(index);
    }  else if (type.str() == "int64[]") {
        runtime::Array<int64_t> array(str, dim, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        return array.getEntry(index);
    } else if (type.str() == "float[]") {
        runtime::Array<float> array(str, dim, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        return array.getEntry(index);
    } else if (type.str() == "double[]") {
        runtime::Array<double> array(str, dim, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        return array.getEntry(index);
    } else if (type.str() == "string[]") {
        runtime::Array<std::string> array(str, dim, &runtime::TypeCasts::stringToString, &runtime::TypeCasts::stringToString);
        return array.getEntry(index);
    } else {
        throw std::runtime_error("The entered type - " + type.str() + " - is currently not supported");
    }
    return str;
}

runtime::VarLen32 runtime::ArrayRuntime::getDimensions(runtime::VarLen32 str, uint64_t dim, runtime::VarLen32 type) {
    if (type.str() == "int32[]") {
        runtime::Array<int32_t> array(str, dim, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        return  array.getDimensionRange();
    }  else if (type.str() == "int64[]") {
        runtime::Array<int64_t> array(str, dim, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        return  array.getDimensionRange();
    } else if (type.str() == "float[]") {
        runtime::Array<float> array(str, dim, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        return  array.getDimensionRange();
    } else if (type.str() == "double[]") {
        runtime::Array<double> array(str, dim, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        return  array.getDimensionRange();
    } else if (type.str() == "string[]") {
        runtime::Array<std::string> array(str, dim, &runtime::TypeCasts::stringToString, &runtime::TypeCasts::stringToString);
        return  array.getDimensionRange();
    } else {
        throw std::runtime_error("The entered type - " + type.str() + " - is currently not supported");
    }
    return str;
}

uint64_t runtime::ArrayRuntime::getCardinality(runtime::VarLen32 str, uint64_t dim, runtime::VarLen32 type) {
    if (type.str() == "int32[]") {
        runtime::Array<int32_t> array(str, dim, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        return  array.getCardinality();
    }  else if (type.str() == "int64[]") {
        runtime::Array<int64_t> array(str, dim, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        return  array.getCardinality();
    } else if (type.str() == "float[]") {
        runtime::Array<float> array(str, dim, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        return  array.getCardinality();
    } else if (type.str() == "double[]") {
        runtime::Array<double> array(str, dim, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        return  array.getCardinality();
    } else if (type.str() == "string[]") {
        runtime::Array<std::string> array(str, dim, &runtime::TypeCasts::stringToString, &runtime::TypeCasts::stringToString);
        return  array.getCardinality();
    } else {
        throw std::runtime_error("The entered type - " + type.str() + " - is currently not supported");
    }
    return 0;
}

runtime::VarLen32 runtime::ArrayRuntime::fill(runtime::VarLen32 str, uint64_t dim, runtime::VarLen32 value, runtime::VarLen32 valueType) {
    runtime::Array<int64_t> arrayData(str, dim, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
    if (valueType.str() == "int32") {
        runtime::Array<int32_t> result("{}", 1, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        result.fill(arrayData, value.str(), &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        return result.toString();
    } else if (valueType.str() == "int64") {
        runtime::Array<int64_t> result("{}", 1, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        result.fill(arrayData, value.str(), &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        return result.toString();
    } else if (valueType.str() == "float") {
        runtime::Array<float> result("{}", 1, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        result.fill(arrayData, value.str(), &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        return result.toString();
    } else if (valueType.str() == "double") {
        runtime::Array<double> result("{}", 1, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        result.fill(arrayData, value.str(), &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        return result.toString();
    } else if (valueType.str() == "string") {
        runtime::Array<std::string> result("{}", 1, &runtime::TypeCasts::stringToString, &runtime::TypeCasts::stringToString);
        result.fill(arrayData, value.str(), &runtime::TypeCasts::stringToString, &runtime::TypeCasts::stringToString);
        return result.toString();
    } else {
        throw std::runtime_error("The entered value is not supported to be used for array_fill");
    }
}

runtime::VarLen32 runtime::ArrayRuntime::incDim(runtime::VarLen32 str, uint64_t dim, runtime::VarLen32 type) {
    if (type.str() == "int32[]") {
        runtime::Array<int32_t> array(str, dim, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        array.incrementDimension();
        return  array.toString();
    }  else if (type.str() == "int64[]") {
        runtime::Array<int64_t> array(str, dim, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        array.incrementDimension();
        return  array.toString();
    } else if (type.str() == "float[]") {
        runtime::Array<float> array(str, dim, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        array.incrementDimension();
        return  array.toString();
    } else if (type.str() == "double[]") {
        runtime::Array<double> array(str, dim, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        array.incrementDimension();
        return  array.toString();
    } else if (type.str() == "string[]") {
        runtime::Array<std::string> array(str, dim, &runtime::TypeCasts::stringToString, &runtime::TypeCasts::stringToString);
        array.incrementDimension();
        return  array.toString();
    } else {
        throw std::runtime_error("The entered type - " + type.str() + " - is currently not supported");
    }
    return str;
}

runtime::VarLen32 runtime::ArrayRuntime::add(runtime::VarLen32 str1, uint64_t dim1, runtime::VarLen32 type1, runtime::VarLen32 str2, uint64_t dim2, runtime::VarLen32 type2) {
    if (type1.str() != type2.str()) {
        throw std::runtime_error("Both arrays should have the same type - Type-1: " + type1.str() + "; Type-2: " + type2.str());
    }

    if (type1.str() == "int32[]") {
        runtime::Array<int32_t> array1(str1, dim1, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        runtime::Array<int32_t> array2(str2, dim2, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        array1.add(array2);
        return array1.toString();
    }  else if (type1.str() == "int64[]") {
        runtime::Array<int64_t> array1(str1, dim1, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        runtime::Array<int64_t> array2(str2, dim2, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        array1.add(array2);
        return array1.toString();
    } else if (type1.str() == "float[]") {
        runtime::Array<float> array1(str1, dim1, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        runtime::Array<float> array2(str2, dim2, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        array1.add(array2);
        return array1.toString();
    } else if (type1.str() == "double[]") {
        runtime::Array<double> array1(str1, dim1, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        runtime::Array<double> array2(str2, dim2, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        array1.add(array2);
        return array1.toString();
    } else {
        throw std::runtime_error("The addition operator is only supported for numeric arrays");
    }
}

runtime::VarLen32 runtime::ArrayRuntime::sub(runtime::VarLen32 str1, uint64_t dim1, runtime::VarLen32 type1, runtime::VarLen32 str2, uint64_t dim2, runtime::VarLen32 type2) {
    if (type1.str() != type2.str()) {
        throw std::runtime_error("Both arrays should have the same type - Type-1: " + type1.str() + "; Type-2: " + type2.str());
    }

    if (type1.str() == "int32[]") {
        runtime::Array<int32_t> array1(str1, dim1, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        runtime::Array<int32_t> array2(str2, dim2, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        array1.sub(array2);
        return array1.toString();
    }  else if (type1.str() == "int64[]") {
        runtime::Array<int64_t> array1(str1, dim1, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        runtime::Array<int64_t> array2(str2, dim2, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        array1.sub(array2);
        return array1.toString();
    } else if (type1.str() == "float[]") {
        runtime::Array<float> array1(str1, dim1, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        runtime::Array<float> array2(str2, dim2, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        array1.sub(array2);
        return array1.toString();
    } else if (type1.str() == "double[]") {
        runtime::Array<double> array1(str1, dim1, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        runtime::Array<double> array2(str2, dim2, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        array1.sub(array2);
        return array1.toString();
    } else {
        throw std::runtime_error("The subtraction operator is only supported for numeric arrays");
    }
}

runtime::VarLen32 runtime::ArrayRuntime::mulEW(runtime::VarLen32 str1, uint64_t dim1, runtime::VarLen32 type1, runtime::VarLen32 str2, uint64_t dim2, runtime::VarLen32 type2) {
    if (type1.str() != type2.str()) {
        throw std::runtime_error("Both arrays should have the same type - Type-1: " + type1.str() + "; Type-2: " + type2.str());
    }

    if (type1.str() == "int32[]") {
        runtime::Array<int32_t> array1(str1, dim1, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        runtime::Array<int32_t> array2(str2, dim2, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        array1.mulEW(array2);
        return array1.toString();
    }  else if (type1.str() == "int64[]") {
        runtime::Array<int64_t> array1(str1, dim1, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        runtime::Array<int64_t> array2(str2, dim2, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        array1.mulEW(array2);
        return array1.toString();
    } else if (type1.str() == "float[]") {
        runtime::Array<float> array1(str1, dim1, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        runtime::Array<float> array2(str2, dim2, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        array1.mulEW(array2);
        return array1.toString();
    } else if (type1.str() == "double[]") {
        runtime::Array<double> array1(str1, dim1, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        runtime::Array<double> array2(str2, dim2, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        array1.mulEW(array2);
        return array1.toString();
    } else {
        throw std::runtime_error("The multiplication operator is only supported for numeric arrays");
    }
}

runtime::VarLen32 runtime::ArrayRuntime::matrixMul(runtime::VarLen32 str1, uint64_t dim1, runtime::VarLen32 type1, runtime::VarLen32 str2, uint64_t dim2, runtime::VarLen32 type2) {
    if (type1.str() != type2.str()) {
        throw std::runtime_error("Both arrays should have the same type - Type-1: " + type1.str() + "; Type-2: " + type2.str());
    }

    if (type1.str() == "int32[]") {
        runtime::Array<int32_t> array1(str1, dim1, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        runtime::Array<int32_t> array2(str2, dim2, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        return array1.matrixMul(array2);
    }  else if (type1.str() == "int64[]") {
        runtime::Array<int64_t> array1(str1, dim1, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        runtime::Array<int64_t> array2(str2, dim2, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        return array1.matrixMul(array2);
    } else if (type1.str() == "float[]") {
        runtime::Array<float> array1(str1, dim1, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        runtime::Array<float> array2(str2, dim2, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        return array1.matrixMul(array2);
    } else if (type1.str() == "double[]") {
        runtime::Array<double> array1(str1, dim1, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        runtime::Array<double> array2(str2, dim2, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        return array1.matrixMul(array2);
    } else {
        throw std::runtime_error("The matrix multiplication operator is only supported for numeric arrays");
    }
}

runtime::VarLen32 runtime::ArrayRuntime::scalarMultInt(runtime::VarLen32 str, uint64_t dim, runtime::VarLen32 type, int64_t scalar) {
    if (type.str() == "int32[]") {
        runtime::Array<int32_t> array(str, dim, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        array.scalarMult(scalar);
        return  array.toString();
    }  else if (type.str() == "int64[]") {
        runtime::Array<int64_t> array(str, dim, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        array.scalarMult(scalar);
        return  array.toString();
    } else if (type.str() == "float[]") {
        runtime::Array<float> array(str, dim, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        array.scalarMult(scalar);
        return  array.toString();
    } else if (type.str() == "double[]") {
        runtime::Array<double> array(str, dim, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        array.scalarMult(scalar);
        return  array.toString();
    } else {
        throw std::runtime_error("The scalar-multiplication operator is only supported for numeric arrays");
    }
    return str;
}

runtime::VarLen32 runtime::ArrayRuntime::scalarMultFloat(runtime::VarLen32 str, uint64_t dim, runtime::VarLen32 type, double scalar) {
    if (type.str() == "int32[]") {
        runtime::Array<int32_t> array(str, dim, &runtime::TypeCasts::stringToInt32, &runtime::TypeCasts::numericToString<int32_t>);
        array.scalarMult(scalar);
        return  array.toString();
    }  else if (type.str() == "int64[]") {
        runtime::Array<int64_t> array(str, dim, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);
        array.scalarMult(scalar);
        return  array.toString();
    } else if (type.str() == "float[]") {
        runtime::Array<float> array(str, dim, &runtime::TypeCasts::stringToFloat, &runtime::TypeCasts::numericToString<float>);
        array.scalarMult(scalar);
        return  array.toString();
    } else if (type.str() == "double[]") {
        runtime::Array<double> array(str, dim, &runtime::TypeCasts::stringToDouble, &runtime::TypeCasts::numericToString<double>);
        array.scalarMult(scalar);
        return  array.toString();
    } else {
        throw std::runtime_error("The scalar-multiplication operator is only supported for numeric arrays");
    }
    return str;
}

runtime::VarLen32 runtime::ArrayRuntime::transpose(runtime::VarLen32 str, uint64_t dim, runtime::VarLen32 type) {
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

    runtime::Array<int64_t> reconstructedArrayData(formattedArrayDim, dim, &runtime::TypeCasts::stringToInt64, &runtime::TypeCasts::numericToString<int64_t>);

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
}