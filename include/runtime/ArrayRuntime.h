#ifndef RUNTIME_ARRAYRUNTIME_H
#define RUNTIME_ARRAYRUNTIME_H
#include "runtime/helpers.h"
#include <vector>

namespace runtime
{
    struct ArrayRuntime{
        static void toArray(VarLen32 array, std::vector<int32_t>& container);
    };
} // namespace runtime


#endif // RUNTIME_ARRAYRUNTIME_H