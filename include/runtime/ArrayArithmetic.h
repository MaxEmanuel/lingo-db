#include <cstdint>
#include <string>
#include <cblas.h>

namespace runtime {

struct ArrayAddOperator {

    /**
     * This function executes an addition operation on a list of values of type `TYPE` and
     * copies the result in the given buffer.
     * 
     * @param left A pointer to a value of type `TYPE`.
     * @param right A pointer to a value of type `TYPE`.
     * @param size The length of both value lists.
     * @param buffer A reference to a char pointer which points to the string
     * that should store the result.
     * @param scalarLeft If the left parameter points to a single element.
     * @param scalarRight If the right parameter points to a single element.
     */
    template<class TYPE>
    static void Operator(const TYPE *left, const TYPE *right, uint32_t size, char *&buffer, bool scalarLeft, bool scalarRight) {
        for (size_t i = 0; i < size; i++) {
            TYPE result = *left + *right;
            memcpy(buffer, &result, sizeof(TYPE));
            buffer += sizeof(TYPE);
            if (!scalarLeft) left++;
            if (!scalarRight) right++;
        }
    }
};

struct ArraySubOperator{

    /**
     * This function executes a subtraction operation on a list of values of type `TYPE` and
     * copies the result in the given buffer.
     * 
     * @param left A pointer to a value of type `TYPE`.
     * @param right A pointer to a value of type `TYPE`.
     * @param size The length of both value lists.
     * @param buffer A reference to a char pointer which points to the string
     * that should store the result.
     * @param scalarLeft If the left parameter points to a single element.
     * @param scalarRight If the right parameter points to a single element.
     */
    template<class TYPE>
    static void Operator(const TYPE *left, const TYPE *right, uint32_t size, char *&buffer, bool scalarLeft, bool scalarRight) {
        for (size_t i = 0; i < size; i++) {
            TYPE result = *left - *right;
            memcpy(buffer, &result, sizeof(TYPE));
            buffer += sizeof(TYPE);
            if (!scalarLeft) left++;
            if (!scalarRight) right++;
        }
    }
};

struct ArrayMulOperator{

    /**
     * This function executes an multiplication operation on a list of values of type `TYPE` and
     * copies the result in the given buffer.
     * 
     * @param left A pointer to a value of type `TYPE`.
     * @param right A pointer to a value of type `TYPE`.
     * @param size The length of both value lists.
     * @param buffer A reference to a char pointer which points to the string
     * that should store the result.
     * @param scalarLeft If the left parameter points to a single element.
     * @param scalarRight If the right parameter points to a single element.
     */
    template<class TYPE>
    static void Operator(const TYPE *left, const TYPE *right, uint32_t size, char *&buffer, bool scalarLeft, bool scalarRight) {
        for (size_t i = 0; i < size; i++) {
            TYPE result = *left * *right;
            memcpy(buffer, &result, sizeof(TYPE));
            buffer += sizeof(TYPE);
            if (!scalarLeft) left++;
            if (!scalarRight) right++;
        }
    }
};

struct ArrayDivOperator{

    /**
     * This function executes an division operation on a list of values of type `TYPE` and
     * copies the result in the given buffer.
     * 
     * @param left A pointer to a value of type `TYPE`.
     * @param right A pointer to a value of type `TYPE`.
     * @param size The length of both value lists.
     * @param buffer A reference to a char pointer which points to the string
     * that should store the result.
     * @param scalarLeft If the left parameter points to a single element.
     * @param scalarRight If the right parameter points to a single element.
     */
    template<class TYPE>
    static void Operator(const TYPE *left, const TYPE *right, uint32_t size, char *&buffer, bool scalarLeft, bool scalarRight) {
        for (size_t i = 0; i < size; i++) {
            TYPE result = *left / *right;
            memcpy(buffer, &result, sizeof(TYPE));
            buffer += sizeof(TYPE);
            if (!scalarLeft) left++;
            if (!scalarRight) right++;
        }
    }

};

template <class TYPE, class RETURN_TYPE>
inline void Gemm(int rowsA, int columnsB, int rowsB, const TYPE *A, const TYPE *B, RETURN_TYPE *C) {
    static_assert(sizeof(TYPE) == 0, "Gemm not implemented for this type");
}

template <>
inline void Gemm<double, double>(int rowsA, int columnsB, int rowsB, const double *A, const double *B, double *C) {
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, rowsA, columnsB, rowsB, 1.0, A, rowsB, B, columnsB, 0.0, C, columnsB);
}

template <>
inline void Gemm<float, float>(int rowsA, int columnsB, int rowsB, const float *A, const float *B, float *C) {
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, rowsA, columnsB, rowsB, 1.0f, A, rowsB, B, columnsB, 0.0f, C, columnsB);
}

template <>
inline void Gemm<uint16_t, float>(int rowsA, int columnsB, int rowsB, const uint16_t *A, const uint16_t *B, float *C) {
    cblas_sbgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, rowsA, columnsB, rowsB, 1.0f, A, rowsB, B, columnsB, 0.0f, C, columnsB);
}

struct MatrixMultiplicationOperator {

    /**
     * This function executes a matrix multiplication operation with lists of values of type `TYPE` and
     * copies the result in the given buffer.
     * 
     * @param left A pointer to a value of type `TYPE`.
     * @param right A pointer to a value of type `TYPE`.
     * @param rowsA The number of rows of the left matrix.
     * @param rowsB The number of rows of the right matrix.
     * @param columnsB The number of columns of the right matrix.
     * @param buffer A reference to a char pointer which points to the string
     * that should store the result.
     */
	template <class TYPE>
	static void Operator(const TYPE *left, const TYPE *right, uint32_t rowsA, uint32_t rowsB, uint32_t columnsB, char *&buffer) {
		size_t sizeC = rowsA * columnsB;
		std::vector<TYPE> result;
		result.reserve(sizeC);

		Gemm<TYPE, TYPE>(rowsA, columnsB, rowsB, left, right, result.data());

		memcpy(buffer, result.data(), sizeof(TYPE) * sizeC);
        buffer += sizeof(TYPE) * sizeC;
	}
};

struct ArraySigmoidOperator {

    /**
     * This function executes the sigmoid function on a list of values of type `TYPE` and
     * copies the result in the given buffer.
     * 
     * @param data A pointer to the first entry of a list of values.
     * @param size The length of the given list.
     * @param buffer A reference to a char pointer which points to the string
     * that should store the result.
     */
    template <class TYPE>
	static void Operator(const TYPE *data, uint32_t size, char *&buffer) {
		for(size_t i = 0; i < size; i++) {
			TYPE result = 1 / (1 + std::exp(-(*data++)));
            memcpy(buffer, &result, sizeof(TYPE));
            buffer += sizeof(TYPE);
		}
	}
};

}