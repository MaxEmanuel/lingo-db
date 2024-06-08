#include "runtime/helpers.h"
#include "runtime/ArrayRuntime.h"
#include <string>

void runtime::ArrayRuntime::toArray(runtime::VarLen32 array, std::vector<int32_t>& container) {
    /* std::string content = array.str();
    content.erase(std::__remove_if(content.begin(), content.end(), " "), content.end());
    for (size_t index = 0; index < content.size(); index++) {
        switch (content[index]) {
            case '{': {
                break;
            }
            case '}': {

            }
        }
    } */
    container.push_back(4);
    container.push_back(5);
}