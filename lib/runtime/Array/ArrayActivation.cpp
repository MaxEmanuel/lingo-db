#include "runtime/Array.h"

using runtime::Array;

template<>
void runtime::ArraySigmoidOperator::Operator<__bf16>(const __bf16 *data, uint32_t size, char *&buffer) {
    for(size_t i = 0; i < size; i++) {
		float floatResult = 1 / (1 + std::exp(-(static_cast<float>(*data++))));
        auto result = static_cast<__bf16>(floatResult);
        memcpy(buffer, &result, sizeof(__bf16));
        buffer += sizeof(__bf16);
	}
}

runtime::VarLen32 Array::sigmoid() {
    if (!isNumericType(this->type)) {
        throw std::runtime_error("Array-Sigmoid: Only supported with numeric types");
    }
    return executeActivationFunction<ArraySigmoidOperator>();
}