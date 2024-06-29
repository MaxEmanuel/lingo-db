#include "runtime/helpers.h"
#include "runtime/ArrayRuntime.h"

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

runtime::VarLen32 runtime::ArrayRuntime::getRange(runtime::VarLen32 str, int dim, runtime::VarLen32 type, int start, int stop) {
    std::runtime_error("Feature is currently unavailable");
    /* if (type.str() == "int32[]") {
        runtime::Array<int32_t> array(str, dim, &runtime::Array<int32_t>::stringToInt32);
        array.setToRange(start, stop);
        return array.toString(&runtime::Array<int32_t>::numericToString<int32_t>);
    }  else if (type.str() == "int64[]") {
        runtime::Array<int64_t> array(str, dim, &runtime::Array<int64_t>::stringToInt64);
        return array.toString(&runtime::Array<int64_t>::numericToString<int64_t>);
        array.setToRange(start, stop);
    } else if (type.str() == "float[]") {
        runtime::Array<float> array(str, dim, &runtime::Array<float>::stringToFloat);
        array.setToRange(start, stop);
        return array.toString(&runtime::Array<float>::numericToString<float>);
    } else if (type.str() == "double[]") {
        runtime::Array<double> array(str, dim, &runtime::Array<double>::stringToDouble);
        array.setToRange(start, stop);
        return array.toString(&runtime::Array<double>::numericToString<double>);
    } else if (type.str() == "string[]") {
        runtime::Array<std::string> array(str, dim, &runtime::Array<std::string>::stringToArrayString);
        array.setToRange(start, stop);
        return array.toString(&runtime::Array<std::string>::arrayStringToString);
    } else {
        throw std::runtime_error("The entered type - " + type.str() + " - is currently not supported");
    } */
}

runtime::VarLen32 runtime::ArrayRuntime::getEntry(runtime::VarLen32 str, int dim, runtime::VarLen32 type, int index) {
    std::runtime_error("Feature is currently unavailable");
    /* if (type.str() == "int32[]") {
        runtime::Array<int32_t> array(str, dim, &runtime::Array<int32_t>::stringToInt32);
        array.setToElement(index);
        return array.toString(&runtime::Array<int32_t>::numericToString<int32_t>);
    }  else if (type.str() == "int64[]") {
        runtime::Array<int64_t> array(str, dim, &runtime::Array<int64_t>::stringToInt64);
        array.setToElement(index);
        return array.toString(&runtime::Array<int64_t>::numericToString<int64_t>);
    } else if (type.str() == "float[]") {
        runtime::Array<float> array(str, dim, &runtime::Array<float>::stringToFloat);
        array.setToElement(index);
        return array.toString(&runtime::Array<float>::numericToString<float>);
    } else if (type.str() == "double[]") {
        runtime::Array<double> array(str, dim, &runtime::Array<double>::stringToDouble);
        array.setToElement(index);
        return array.toString(&runtime::Array<double>::numericToString<double>);
    } else if (type.str() == "string[]") {
        runtime::Array<std::string> array(str, dim, &runtime::Array<std::string>::stringToArrayString);
        array.setToElement(index);
        return array.toString(&runtime::Array<std::string>::arrayStringToString);
    } else {
        throw std::runtime_error("The entered type - " + type.str() + " - is currently not supported");
    } */
}

runtime::VarLen32 runtime::ArrayRuntime::getDimensions(runtime::VarLen32 str, int dim, runtime::VarLen32 type) {
    std::runtime_error("Feature is currently unavailable");
    /* if (type.str() == "int32[]") {
        runtime::Array<int32_t> array(str, dim, &runtime::Array<int32_t>::stringToInt32);
        return array.getDimensions();
    }  else if (type.str() == "int64[]") {
        runtime::Array<int64_t> array(str, dim, &runtime::Array<int64_t>::stringToInt64);
        return array.getDimensions();
    } else if (type.str() == "float[]") {
        runtime::Array<float> array(str, dim, &runtime::Array<float>::stringToFloat);
        return array.getDimensions();
    } else if (type.str() == "double[]") {
        runtime::Array<double> array(str, dim, &runtime::Array<double>::stringToDouble);
        return array.getDimensions();
    } else if (type.str() == "string[]") {
        runtime::Array<std::string> array(str, dim, &runtime::Array<std::string>::stringToArrayString);
        return array.getDimensions();
    } else {
        throw std::runtime_error("The entered type - " + type.str() + " - is currently not supported");
    } */
}

runtime::VarLen32 runtime::ArrayRuntime::getCardinality(runtime::VarLen32 str, int dim, runtime::VarLen32 type) {
    std::runtime_error("Feature is currently unavailable");
    /* if (type.str() == "int32[]") {
        runtime::Array<int32_t> array(str, dim, &runtime::Array<int32_t>::stringToInt32);
        return array.getCardinality();
    }  else if (type.str() == "int64[]") {
        runtime::Array<int64_t> array(str, dim, &runtime::Array<int64_t>::stringToInt64);
        return array.getCardinality();
    } else if (type.str() == "float[]") {
        runtime::Array<float> array(str, dim, &runtime::Array<float>::stringToFloat);
        return array.getCardinality();
    } else if (type.str() == "double[]") {
        runtime::Array<double> array(str, dim, &runtime::Array<double>::stringToDouble);
        return array.getCardinality();
    } else if (type.str() == "string[]") {
        runtime::Array<std::string> array(str, dim, &runtime::Array<std::string>::stringToArrayString);
        return array.getCardinality();
    } else {
        throw std::runtime_error("The entered type - " + type.str() + " - is currently not supported");
    } */
}