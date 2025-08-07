#include "runtime/Array.h"

using runtime::Array;

runtime::VarLen32 Array::fromString(std::string &source, int32_t type) {
    uint8_t typeId = getTypeId(type);
    uint32_t dimensions = 0;
    std::vector<int32_t> indices;
    std::vector<uint32_t> lengths;
    // How many characters should be ignored (possible header)
    auto start = parseHeader(source, indices, lengths);
    // If header exists, dimensions can be derived
    dimensions = indices.size();

    if (source.begin() + start == source.end()) {
        throw std::runtime_error("Array-Cast: Invalid array specification");
    }

    uint32_t position = 0;
    // Stores the dimension in which the elements are stored
    uint32_t lastDimension = dimensions;
    // Index to know which metadata entry should be updated
    uint32_t widthIndex = 0;
    uint32_t widthOffset = 0;
    std::vector<std::string> elements;
    std::vector<uint32_t> stringLengths;
    std::vector<uint32_t> widthMap;
    std::vector<uint32_t> widths;
    std::vector<bool> nulls;
    // Iterate over each character from the given string
    for(auto symbol = source.begin() + start; symbol != source.end(); symbol++) {
        switch(*symbol) {
            // Enter next lower dimension
            case '{':
            {
                // Restrict possiblity of infinite number of empty arrays
                if (lastDimension != 0 && lastDimension < position+1) {
                    throw std::runtime_error("Array-Cast: Invalid structure, elements are not in lowest dimension");
                }
                // If not in first dimension, update width of upper dimension
                if (widths.size() != 0) {
                    widths[widthIndex] += 1;
                    // Check if new structure is inside defined bounds (Only if header is defined)
                    if (lengths.size() != 0) {
                        if (widths[widthIndex] > lengths[position-1]) {
                            throw std::runtime_error("Array-Cast: Invalid structure, array object is out of bounds");
                        }
                    }
                }
                // Determine the position to add a new width
                auto insertPosition = widths.begin();
                for (size_t i = 0; i < widthMap.size(); i++) {
                    if (i > position) break;
                    insertPosition += widthMap[i];
                }
                position++;
                // Add new width entry
                insertPosition = widths.insert(insertPosition, 0);
                widthIndex = insertPosition - widths.begin();
                // Add new widthMap information if completely new dimension has been discovered
                if (position > widthMap.size()) {
                    widthMap.push_back(1);
                // Otherwise increase value of already existing one
                } else {
                    widthMap[position - 1] += 1;
                }
                // Add start index if not already defined in a header
                if (position > indices.size()) {
                    indices.push_back(1);
                }
                // Update absolute number of dimensions
                dimensions = std::max(dimensions, position);
                widthOffset = nulls.size();
                break;
            }
            // Enter previous upper dimension
            case '}':
            {
                // Update width entry only of last dimension
                if (lastDimension == position) {
                    widths[widthIndex] = nulls.size() - widthOffset;
                    // Check if new structure is inside defined bounds (Only if header is defined)
                    if (lengths.size() != 0) {
                        if (widths[widthIndex] > lengths[position-1]) {
                            throw std::runtime_error("Array-Cast: Invalid structure, array object is out of bounds"); 
                        }
                    }
                }
                position--;
                // Determine the index of the last width entry that corresponds to the upper dimension
                auto modifyPosition = widths.begin();
                for (size_t i = 0; i < widthMap.size(); i++) {
                    if (i == position) break;
                    modifyPosition += widthMap[i];
                }
                if (modifyPosition != widths.begin()) {
                    modifyPosition--;
                    widthIndex = modifyPosition - widths.begin();
                }
                break;
            }
            // Only check if syntax is correct
            case ',':
            {
                while(symbol < source.end() && isspace(*(symbol + 1))) {
                    symbol++;
                }
                if (symbol + 1 < source.end() && (*(symbol + 1) == ',' || *(symbol + 1) == '}')) {
                    throw std::runtime_error("Array-Cast: Syntax error");
                }
                break;
            }
            // Check if NULL value has been discovered
            case 'n':
            case 'N':
            {
                if (symbol + 4 < source.end()) {
                    char possibleU = *(symbol + 1);
                    char possibleL1 = *(symbol + 2);
                    char possibleL2 = *(symbol + 3);
                    if ((possibleU == 'u' || possibleU == 'U') && (possibleL1 == 'l' || possibleL1 == 'L') && (possibleL2 == 'l' || possibleL2 == 'L')) {
                        if (lastDimension == 0) {
                            lastDimension = position;
                        }
                        if (lastDimension != position) {
                            throw std::runtime_error("Array-Cast: NULL can only be used for primitive elements");
                        }
                        symbol += 3;
                        nulls.push_back(true);
                    }
                }
                break;
            }
            // Ignore these special characters
            case ' ':
            case '\\':
            case '\t':
            case '\n':
            case '\r':
                break;
            // Default = New array element discovered
            default:
            {
                // Check if no other characters are defined outside the array
                if (position == 0) {
                    throw std::runtime_error("Array-Cast: Invalid array specification");
                }
                // Check if all elements are at the same dimension level
                if (lastDimension != 0 && lastDimension != position) {
                    throw std::runtime_error("Array-Cast: Inconsistent dimensions - found elements in different dimensions");
                }
                lastDimension = position;
                // Jump over " if present
                auto end = symbol;
                if (*symbol == '"') {
                    symbol++;
                    end++;
                }
                // Find end of array element
                while (end < source.end()) {
                    if (*end == '"' && *(end - 1) != '\\') {
                        break;
                    }
                    if (*end == ',' && *(end - 1) != '\\') {
                        break;
                    }
                    if (*end == '}' && *(end - 1) != '\\') {
                        break;
                    }
                    end++;
                }
                // Add discovered element
                elements.push_back(std::string(symbol, end));
                // If type is STRING, also store the elements length
                if (type == ArrayType::STRING) {
                    stringLengths.push_back(end - symbol);
                    symbol = end;
                } else {
                    symbol = end - 1;
                }
                nulls.push_back(false);
                break;
            }
        }
    }
    // Get total size of all string values (if type is STRING)
    size_t totalStringSize = 0;
    for (auto &length : stringLengths) {
        totalStringSize += length;
    }
    std::string result;
    // Set new size of target string
    auto size = getStringSize(dimensions, elements.size(), widths.size(), getNullBytes(nulls.size()), totalStringSize, typeId);
    result.resize(size);
    char *buffer = result.data();
    
    // Now copy each information into the target string
    writeToBuffer(buffer, ARRAYHEADER.data(), ARRAYHEADER.size());
    writeToBuffer(buffer, &typeId, 1);
    writeToBuffer(buffer, &dimensions, 1);
    uint32_t elementSize = elements.size();
    writeToBuffer(buffer, &elementSize, 1);
    writeToBuffer(buffer, indices.data(), indices.size());
    writeToBuffer(buffer, widthMap.data(), widthMap.size());
    writeToBuffer(buffer, widths.data(), widths.size());

    if (typeId == ArrayType::STRING) {
        writeToBuffer(buffer, stringLengths.data(), stringLengths.size());
    } else {
        for (auto &element : elements) {
            if (typeId == ArrayType::INTEGER32) {
                castAndCopyElement<int32_t>(buffer, element);
            } else if (typeId == ArrayType::INTEGER64) {
                castAndCopyElement<int64_t>(buffer, element);
            } else if (typeId == ArrayType::BFLOAT) {
                castAndCopyElement<__bf16>(buffer, element);
            } else if (typeId == ArrayType::FLOAT) {
                castAndCopyElement<float>(buffer, element);
            } else {
                castAndCopyElement<double>(buffer, element);
            }
        }
    }

    copyNulls(buffer, nulls);

    if (typeId == ArrayType::STRING) {
        for (auto &element : elements) {
            writeToBuffer(buffer, element.data(), element.length());
        }
    }
    return toVarLen32(result);
}

uint32_t Array::parseHeader(std::string &array, std::vector<int32_t> &indices, std::vector<uint32_t> &lengths) {
    auto symbol = array.begin();
    bool foundHeader = false;
    try {
        // Iterate over each character
        while (symbol < array.end() && *symbol != '=' && *symbol != '{') {
            switch (*symbol) {
                // First character should be a '['
                case '[':
                {
                    foundHeader = true;
                    // Move to the first header entry value
                    symbol++;
                    while (symbol < array.end() && isspace(*symbol)) symbol++;
                    std::string value(1, *symbol);
                    symbol++;
                    // Identify all characters of that number (Must be a number)
                    while (symbol < array.end() && *symbol != ':') {
                        if (*symbol != ':' && !isdigit(*symbol) && !isspace(*symbol)) {
                            throw std::runtime_error("Array-Cast: Invalid array header");
                        }
                        value += *symbol;
                        symbol++;
                    }
                    symbol++;
                    // Add it to the indices list
                    indices.push_back(std::stoi(value));
                    // Move to the second header entry value
                    while (symbol < array.end() && isspace(*symbol)) symbol++;
                    value = std::string(1, *symbol);
                    symbol++;
                    // Identify all characters of that number (Must be a number)
                    while (symbol < array.end() && *symbol != ']') {
                        if (*symbol != ']' && !isdigit(*symbol) && !isspace(*symbol)) {
                            throw std::runtime_error("Array-Cast: Invalid array header");
                        }
                        value += *symbol;
                        symbol++;
                    }
                    symbol++;
                    // Calculate the total length of this dimension
                    int32_t lower = indices.back();
                    int32_t upper = std::stoi(value);
                    if (lower > upper) {
                        throw std::runtime_error("Array-Cast: Invalid Indices in array header");
                    }
                    if (lower > upper) std::swap(lower, upper);
                    lengths.push_back(upper - lower + 1);
                    // Go further to the next header entry / end
                    while (symbol < array.end() && isspace(*symbol)) symbol++;
                    break;
                }
                // Ignore this symbol
                case ' ':
                    symbol++;
                    break;
                default:
                    if (foundHeader) {
                        throw std::runtime_error("Array-Cast: Invalid array header");
                    } else {
                        throw std::runtime_error("Array-Cast: Invalid array specification");
                    }
            }
        }
    } catch (std::out_of_range &exc) {
        throw std::runtime_error("Array-Cast: Specified index is out of range for a 32-bit integer");
    }
    if (*symbol == '=') {
        symbol++;
    }
    return symbol - array.begin();
}

