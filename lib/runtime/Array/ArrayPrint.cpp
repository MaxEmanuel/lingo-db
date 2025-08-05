#include "runtime/Array.h"

using runtime::Array;

std::string Array::print() {
    std::string result = "";
    printHeader(result);
    printArray(result, this->widths, 1);
    return result;
}

void Array::printArray(std::string &target, const uint32_t *width, uint32_t dimension) {
    // Check if last dimension is reached
    if (dimension == this->dimensions) {
        target.append("{");
        uint32_t offset = getOffset(width);
        // Iterate over each element
        for (uint32_t i = offset; i < offset + width[0]; i++) {
            // Check if it is null
            if (isNull(i)) {
                target.append("null");
            } else {
                // Add the value to the result string
                switch (this->type) 
                {
                case ArrayType::INTEGER32:
                    toString<int32_t>(getElementPosition(i), target);
                    break;
                case ArrayType::INTEGER64:
                    toString<int64_t>(getElementPosition(i), target);
                    break;
                case ArrayType::FLOAT:
                    toString<float>(getElementPosition(i), target);
                    break;
                case ArrayType::DOUBLE:
                    toString<double>(getElementPosition(i), target);
                    break;
                case ArrayType::STRING:
                    toString<std::string>(getElementPosition(i), target);
                    break;
                default:
                    throw std::runtime_error("Print-Operation: Given array type is not supported");
                }
            }
            // If not last element, add a comma
            if (i + 1 != offset + width[0]) target.append(",");
        }
        target.append("}");
        return;
    }
    // Get a pointer to the first width entry which belongs to the next lower dimension
    auto *entries = getChildWidth(width, dimension);
    target.append("{");
    // Iterate over each width entry belonging to the caller structure.
    for (uint32_t i = 0; i < width[0]; i++) {
        // Call function with its children
        printArray(target, entries + i, dimension + 1);
        // If not last element, add a comma
        if (i+1 != width[0]) target.append(",");        
    }
    target.append("}");
}

void Array::printHeader(std::string &target) {
    // Check if header must be printed
    auto print = false;
    for (uint32_t i = 0; i < this->dimensions; i++) {
        if (this->indices[i] != 1) print = true;
    }
    if (print) {
        // Iterate over each index and add lower and upper bound to result string
        for (uint32_t i = 0; i < this->dimensions; i++) {
            auto length = getDimensionSize(i+1);
            auto expr = "[" + std::to_string(this->indices[i]) + ":" + std::to_string(this->indices[i] + (int32_t) length - 1) + "]";
            target.append(expr);
        }
        target.append("=");
    }
}