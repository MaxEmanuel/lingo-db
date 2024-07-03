#include "runtime/helpers.h"
#include "runtime/ArrayRuntime.h"
#include "runtime/ArrayRuntime/Array.h"
#include "runtime/ArrayRuntime/TypeCast.h"

runtime::VarLen32 runtime::ArrayRuntime::concat(runtime::VarLen32 str1, int dim1, runtime::VarLen32 type1, runtime::VarLen32 str2, int dim2, runtime::VarLen32 type2) {
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

runtime::VarLen32 runtime::ArrayRuntime::getRange(runtime::VarLen32 str, int dim, runtime::VarLen32 type, int start, int stop, int subDim) {
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

runtime::VarLen32 runtime::ArrayRuntime::getEntry(runtime::VarLen32 str, int dim, runtime::VarLen32 type, int index) {
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

runtime::VarLen32 runtime::ArrayRuntime::getDimensions(runtime::VarLen32 str, int dim, runtime::VarLen32 type) {
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

runtime::VarLen32 runtime::ArrayRuntime::getCardinality(runtime::VarLen32 str, int dim, runtime::VarLen32 type) {
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
    return str;
}

int32_t runtime::ArrayRuntime::castToInt32(runtime::VarLen32 str, int dim, runtime::VarLen32 type) {
    try{
        return runtime::TypeCasts::stringToInt32(str);
    } catch (const std::invalid_argument& exception) {
        std::runtime_error("The given array structure-" + str.str() + "- cannot be converted to a single integer");
    }
}