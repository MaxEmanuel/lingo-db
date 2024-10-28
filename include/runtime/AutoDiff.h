#ifndef RUNTIME_AUTODIFF_H
#define RUNTIME_AUTODIFF_H
#include "runtime/helpers.h"
#include <string>

namespace runtime 
{
    struct AutoDiff{
        /**
         * This function performs automatic differention in reverse mode in order to calculate the derivatives of the given function.
         * @param x a table/dictionary with values for given variables
         * @returns the result of the derived function with  
         */
        static int64_t derivate(int64_t x);
    };

}

#endif // RUNTIME_AUTODIFF_H;