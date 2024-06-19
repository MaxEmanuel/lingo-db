#include "runtime/helpers.h"
#include "runtime/ArrayRuntime.h"

runtime::VarLen32 runtime::ArrayRuntime::concat(runtime::VarLen32 array1, int dim1, runtime::VarLen32 type1, runtime::VarLen32 array2, int dim2, runtime::VarLen32 type2) {
    if (dim1 != dim2) {
        throw std::runtime_error("Both arrays should have the same dimensions - Dim-1: " + std::to_string(dim1) + "; Dim-2: " + std::to_string(dim2));
    }
    if (type1.str() != type2.str()) {
        throw std::runtime_error("Both arrays should have the same type - Type-1: " + type1.str() + "; Type-2: " + type2.str());
    }

    if (type1.str() == "int32[]") {
        runtime::Array<int32_t> finalArray(array1, dim1);
        runtime::Array<int32_t> tmpArray(array2, dim2);
        finalArray.concat(&tmpArray);
        return finalArray.toString();
    /* }  else if (type1.str() == "int64[]") {
        runtime::Array<int64_t> finalArray(array1, dim1);
        runtime::Array<int64_t> tmpArray(array2, dim2);
        finalArray.concat(tmpArray);
        return finalArray.toString();
    } else if (type1.str() == "float[]") {
        runtime::Array<float> finalArray(array1, dim1);
        runtime::Array<float> tmpArray(array2, dim2);
        finalArray.concat(tmpArray);
        return finalArray.toString();
    } else if (type1.str() == "double[]") {
        runtime::Array<double> finalArray(array1, dim1);
        runtime::Array<double> tmpArray(array2, dim2);
        finalArray.concat(tmpArray);
        return finalArray.toString();
    } else if (type1.str() == "string[]") {
        runtime::ArrayString finalArray(array1, dim1);
        runtime::ArrayString tmpArray(array2, dim2);
        finalArray.concat(tmpArray);
        return finalArray.toString(); */
    } else {
        throw std::runtime_error("The entered type - " + type1.str() + " - is currently not supported");
    }
}