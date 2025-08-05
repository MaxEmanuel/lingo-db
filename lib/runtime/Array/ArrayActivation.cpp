#include "runtime/Array.h"

using runtime::Array;

runtime::VarLen32 Array::sigmoid() {
    if (!isNumericType(this->type)) {
        throw std::runtime_error("Array-Sigmoid: Only supported with numeric types");
    }
    return executeActivationFunction<ArraySigmoidOperator>();
}