#define NDEBUG
#include <arrow/array.h>
#include <arrow/visit_array_inline.h>
#include <arrow/pretty_print.h>
#undef NDEBUG

/**
 * This file contains content to extend the Apache Arrow library's pretty-printing capabilities.
 * Currently 16-bit floating point values are printed as 16-bit integer values. This class is a workaround
 * so that 16-bit floating point values can be printed as floating point values. Thereby stored uint16_t
 * values of Apache Arrow are converted to float values (32-bit). 
 * Can be removed if Apache Arrow library is updated to support this.
 * Futhermore this file also ensures that arrays are printed correctly.
 */
namespace execution {

class ExtensionPrinter : public arrow::ArrayVisitor {
    private:
    std::ostream* sink;
    arrow::PrettyPrintOptions options;

    public:
    arrow::Status PrettyPrint(const arrow::ChunkedArray& chunked_arr, arrow::PrettyPrintOptions options, std::ostream* sink);

    arrow::Status Visit(const arrow::HalfFloatArray& array) override;

    arrow::Status Visit(const arrow::StringArray& array) override;

    arrow::Status Visit(const arrow::Array& array);
};

union HalfFloatValue {
    unsigned int asInt;
    float asFloat;
};
}
