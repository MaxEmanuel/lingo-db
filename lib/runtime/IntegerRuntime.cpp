#include "runtime/IntegerRuntime.h"
#include <cassert>
#include <cmath>
#include <random>
int64_t runtime::IntegerRuntime::round64(int64_t value, int64_t roundByScale) {
   assert(roundByScale >= 0);
   return value;
}
int32_t runtime::IntegerRuntime::round32(int32_t value, int64_t roundByScale) {
   assert(roundByScale >= 0);
   return value;
}
int16_t runtime::IntegerRuntime::round16(int16_t value, int64_t roundByScale) {
   assert(roundByScale >= 0);
   return value;
}
int8_t runtime::IntegerRuntime::round8(int8_t value, int64_t roundByScale) {
   assert(roundByScale >= 0);
   return value;
}

int64_t runtime::IntegerRuntime::sqrt(int64_t value) {
   return std::sqrt(value);
}

int32_t runtime::IntegerRuntime::pow(int32_t base, int32_t exp) {
   if(base == 0) return 0;
   if(exp == 0) return 1;
   if(exp < 0) {
      base = 1 / base;
      exp = -exp;
   }
   int32_t result = 1;
   while (exp > 0) {
      if (exp % 2 == 1) {  // If exponent is odd, multiply the result by the base
         result *= base;
      }
      base *= base;
      exp /= 2;
   }
   return result;
}

int64_t runtime::IntegerRuntime::randomInRange(int64_t from, int64_t to) {
   static std::mt19937 gen(0);
   std::uniform_int_distribution<> distr(from, to - 1);
   return distr(gen);
}
