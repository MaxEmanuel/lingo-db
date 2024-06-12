#include "runtime/helpers.h"
#include "runtime/ArrayRuntime.h"
#include <string>
#include <sstream>
#include <algorithm>


#define CAST_STRING_TO_VECTOR(TYPE_NAME, CAST_OP)                                                                       \
   void runtime::ArrayRuntime::toVector(runtime::VarLen32 array, std::vector<std::unique_ptr<TYPE_NAME>>& container) {  \
        std::string content = array.str();                                                                              \
        /* Delete all spaces */                                                                                         \
        content.erase(std::remove_if(content.begin(), content.end(), ::isspace), content.end());                        \
        /* Delete outer spaces ({}) */                                                                                  \
        content = content.substr(1, content.size() - 2);                                                                \
        std::stringstream stringStream(content);                                                                        \
        std::string singleValue;                                                                                        \
        /* Iterate over each single value */                                                                            \
        while(std::getline(stringStream, singleValue, ',')) {                                                           \
            /* Try a cast with the given cast operation */                                                              \
            try {                                                                                                       \
                if (singleValue == "NULL" || singleValue == "null" || singleValue == "Null"){                           \
                    container.push_back(nullptr);                                                                       \
                } else {                                                                                                \
                    container.push_back(std::make_unique<TYPE_NAME>(std::CAST_OP(singleValue)));                        \
                }                                                                                                       \
            } catch (const std::invalid_argument& e){                                                                   \
                throw std::runtime_error("The array value - " + singleValue + " - does not have the correct type");     \
            }                                                                                                           \
                                                                                                                        \
        }                                                                                                               \
   }

CAST_STRING_TO_VECTOR(int32_t, stoi)
CAST_STRING_TO_VECTOR(int64_t, stoll)
CAST_STRING_TO_VECTOR(float, stof)
CAST_STRING_TO_VECTOR(double, stod)

void runtime::ArrayRuntime::toVector(VarLen32 array, std::vector<std::string>& container) {
    std::string content = array.str();
    /* Delete outer spaces ({}) */ 
    content = content.substr(1, content.size() - 2);
    std::stringstream stringStream(content);
    std::string singleValue;
    bool isElement = false;
    /* Iterate over each single value */
    while(std::getline(stringStream, singleValue, '"')) {
        if (isElement) {
            container.push_back(singleValue.substr(1, singleValue.size() - 2));
        } else {
            singleValue.erase(std::remove_if(singleValue.begin(), singleValue.end(), ::isspace), singleValue.end());
            // Proof if some non-string elements were skipped (e.g. {"Hello World", 1, "Test"} whereby '1' has no quotation marks) 
            if (singleValue.size() > 2) {
                throw std::runtime_error("The array value - " + singleValue.substr(1, singleValue.size() - 2) + " - does not have the correct type");
            }
        }
        // Every second '"' contains only the seperator of the elements
        isElement = !isElement;
    }
}

#define CAST_STRING_TO_MATRIX(TYPE_NAME)                                                                                                                \
    void runtime::ArrayRuntime::toMatrix(runtime::VarLen32 array, std::vector<std::unique_ptr<std::vector<std::unique_ptr<TYPE_NAME>>>>& container) {   \
            std::string content = array.str();                                                                                                          \
            /* Remove all spaces */                                                                                                                     \
            content.erase(std::remove_if(content.begin(), content.end(), ::isspace), content.end());                                                    \
            /* Remove outer brackets ({}) */                                                                                                            \
            content = content.substr(1, content.size() - 2);                                                                                            \
            std::stringstream stringStream(content);                                                                                                    \
            std::string singleVector;                                                                                                                   \
            /* Iterate over each vector element */                                                                                                      \
            while(std::getline(stringStream, singleVector, '}')) {                                                                                      \
                if (singleVector == "NULL" || singleVector == "null" || singleVector == "Null") {                                                       \
                    container.push_back(nullptr);                                                                                                       \
                } else {                                                                                                                                \
                    std::unique_ptr<std::vector<std::unique_ptr<TYPE_NAME>>> vector = std::make_unique<std::vector<std::unique_ptr<TYPE_NAME>>>();      \
                    /* Delete "," if it is in front */                                                                                                  \
                    if (singleVector[0] == ',') {                                                                                                       \
                        singleVector = singleVector.substr(1, singleVector.size() - 1);                                                                 \
                    }                                                                                                                                   \
                    if (singleVector[0] != '{') {                                                                                                       \
                        throw std::runtime_error("The elements of the array does not correspond to the number of dimensions specification");            \
                    }                                                                                                                                   \
                    /* Add the deleted "}" from the "getline" function */                                                                               \
                    singleVector = singleVector + '}';                                                                                                  \
                    /* Call the other function which casts a string to an vector */                                                                     \
                    runtime::VarLen32 vectorAsVarLen(reinterpret_cast<const uint8_t*>(singleVector.data()), singleVector.size());                       \
                    runtime::ArrayRuntime::toVector(vectorAsVarLen, *vector);                                                                           \
                    container.push_back(std::move(vector));                                                                                             \
                }                                                                                                                                       \
                                                                                                                                                        \
            }                                                                                                                                           \
    }

CAST_STRING_TO_MATRIX(int32_t)
CAST_STRING_TO_MATRIX(int64_t)
CAST_STRING_TO_MATRIX(float)
CAST_STRING_TO_MATRIX(double)

void runtime::ArrayRuntime::toMatrix(runtime::VarLen32 array, std::vector<std::vector<std::string>>& container) {
    std::string content = array.str();
    /* Remove outer brackets ({}) */
    content = content.substr(1, content.size() - 2);
    std::stringstream stringStream(content);
    std::string singleVector;
    //TODO
}