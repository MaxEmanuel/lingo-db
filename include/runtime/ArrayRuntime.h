#include "runtime/Array.h"

namespace runtime {

    struct ArrayRuntime {

        static VarLen32 fromString(VarLen32 str, int32_t type);

        static VarLen32 getEmtpyArray(int32_t type);

        static VarLen32 appendArray(VarLen32 left, VarLen32 right, int32_t leftType, int32_t rightType);
        static VarLen32 appendInt32(VarLen32 array, int32_t type, int32_t value, bool isFront);
        static VarLen32 appendInt64(VarLen32 array, int32_t type, int64_t value, bool isFront);
        static VarLen32 appendFloat(VarLen32 array, int32_t type, float value, bool isFront);
        static VarLen32 appendDouble(VarLen32 array, int32_t type, double value, bool isFront);
        static VarLen32 appendString(VarLen32 array, int32_t type, VarLen32 value, bool isFront);
        static VarLen32 appendNull(VarLen32 array, int32_t type);

        static VarLen32 slice(VarLen32 array, int32_t type, int32_t lowerBound, int32_t upperBound, int32_t dimension);

        static VarLen32 subscript(VarLen32 array, int32_t type, int32_t position);

        static VarLen32 add(VarLen32 left, VarLen32 right, int32_t leftType, int32_t rightType);
        static VarLen32 sub(VarLen32 left, VarLen32 right, int32_t leftType, int32_t rightType);
        static VarLen32 mul(VarLen32 left, VarLen32 right, int32_t leftType, int32_t rightType);
        static VarLen32 div(VarLen32 left, VarLen32 right, int32_t leftType, int32_t rightType);

        static VarLen32 scalarAddInt32(VarLen32 array, int32_t type, int32_t value);
        static VarLen32 scalarAddInt64(VarLen32 array, int32_t type, int64_t value);
        static VarLen32 scalarAddFloat(VarLen32 array, int32_t type, float value);
        static VarLen32 scalarAddDouble(VarLen32 array, int32_t type, double value);

        static VarLen32 scalarSubInt32(VarLen32 array, int32_t type, int32_t value, bool isLeft);
        static VarLen32 scalarSubInt64(VarLen32 array, int32_t type, int64_t value, bool isLeft);
        static VarLen32 scalarSubFloat(VarLen32 array, int32_t type, float value, bool isLeft);
        static VarLen32 scalarSubDouble(VarLen32 array, int32_t type, double value, bool isLeft);

        static VarLen32 scalarMulInt32(VarLen32 array, int32_t type, int32_t value);
        static VarLen32 scalarMulInt64(VarLen32 array, int32_t type, int64_t value);
        static VarLen32 scalarMulFloat(VarLen32 array, int32_t type, float value);
        static VarLen32 scalarMulDouble(VarLen32 array, int32_t type, double value);

        static VarLen32 scalarDivInt32(VarLen32 array, int32_t type, int32_t value, bool isLeft);
        static VarLen32 scalarDivInt64(VarLen32 array, int32_t type, int64_t value, bool isLeft);
        static VarLen32 scalarDivFloat(VarLen32 array, int32_t type, float value, bool isLeft);
        static VarLen32 scalarDivDouble(VarLen32 array, int32_t type, double value, bool isLeft);

        static VarLen32 matrixMul(VarLen32 left, VarLen32 right, int32_t leftType, int32_t rightType);

        static VarLen32 fillInt32(int32_t value, VarLen32 array, int32_t type);
        static VarLen32 fillInt64(int64_t value, VarLen32 array, int32_t type);
        static VarLen32 fillFloat(float value, VarLen32 array, int32_t type);
        static VarLen32 fillDouble(double value, VarLen32 array, int32_t type);
        static VarLen32 fillString(VarLen32 value, VarLen32 array, int32_t type);
        static VarLen32 fillNull(VarLen32 array, int32_t type);

        static VarLen32 transpose(VarLen32 array, int32_t type);

        static VarLen32 sigmoid(VarLen32 array, int32_t type);

        static int32_t getHighestPosition(VarLen32 array, int32_t type);

        static VarLen32 cast(VarLen32 array, int32_t srcType, int32_t dstType);
        static int32_t toInt32(VarLen32 array);
        static int64_t toInt64(VarLen32 array);
        static float toFloat(VarLen32 array);
        static double toDouble(VarLen32 array);
        static VarLen32 toString(VarLen32 array);

        static VarLen32 increment(VarLen32 array, int32_t type);

        static bool isNull(VarLen32 array);

    };

}