#include "runtime/helpers.h"
#include "runtime/ArrayRuntime.h"

runtime::VarLen32 runtime::ArrayRuntime::concat(runtime::VarLen32 str1, int dim1, runtime::VarLen32 type1, runtime::VarLen32 str2, int dim2, runtime::VarLen32 type2) {
    if (type1.str() != type2.str()) {
        throw std::runtime_error("Both arrays should have the same type - Type-1: " + type1.str() + "; Type-2: " + type2.str());
    }

    if (type1.str() == "int32[]") {
        runtime::Array<int32_t> array1(str1, dim1, &runtime::Array<int32_t>::stringToInt32);
        runtime::Array<int32_t> array2(str2, dim2, &runtime::Array<int32_t>::stringToInt32);
        array1.concat(&array2);
        return array1.toString(&runtime::Array<int32_t>::numericToString<int32_t>);
    }  else if (type1.str() == "int64[]") {
        runtime::Array<int64_t> array1(str1, dim1, &runtime::Array<int64_t>::stringToInt64);
        runtime::Array<int64_t> array2(str2, dim2, &runtime::Array<int64_t>::stringToInt64);
        array1.concat(&array2);
        return array1.toString(&runtime::Array<int64_t>::numericToString<int64_t>);
    } else if (type1.str() == "float[]") {
        runtime::Array<float> array1(str1, dim1, &runtime::Array<float>::stringToFloat);
        runtime::Array<float> array2(str2, dim2, &runtime::Array<float>::stringToFloat);
        array1.concat(&array2);
        return array1.toString(&runtime::Array<float>::numericToString<float>);
    } else if (type1.str() == "double[]") {
        runtime::Array<double> array1(str1, dim1, &runtime::Array<double>::stringToDouble);
        runtime::Array<double> array2(str2, dim2, &runtime::Array<double>::stringToDouble);
        array1.concat(&array2);
        return array1.toString(&runtime::Array<double>::numericToString<double>);
    } else if (type1.str() == "string[]") {
        runtime::Array<std::string> array1(str1, dim1, &runtime::Array<std::string>::stringToArrayString);
        runtime::Array<std::string> array2(str2, dim2, &runtime::Array<std::string>::stringToArrayString);
        array1.concat(&array2);
        return array1.toString(&runtime::Array<std::string>::arrayStringToString);
    } else {
        throw std::runtime_error("The entered type - " + type1.str() + " - is currently not supported");
    }
}