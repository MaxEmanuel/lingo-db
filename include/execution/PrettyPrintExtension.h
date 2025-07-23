#define NDEBUG
#include <arrow/array.h>
#include <arrow/visit_array_inline.h>
#include <arrow/pretty_print.h>
#undef NDEBUG

/**
 * This file contains content to extend the Apache Arrow library's pretty-printing capabilities.
 * It is necessary to be able to distinguish between a normal string and an array string.
 */
namespace execution {

class ExtensionPrinter : public arrow::ArrayVisitor {
    private:
    std::ostream* sink;
    arrow::PrettyPrintOptions options;

    public:
    arrow::Status PrettyPrint(const arrow::ChunkedArray& chunked_arr, arrow::PrettyPrintOptions options, std::ostream* sink);

    arrow::Status Visit(const arrow::StringArray& array) override;

    arrow::Status Visit(const arrow::Array& array);
};
}
