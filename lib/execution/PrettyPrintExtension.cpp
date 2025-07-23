#include "execution/PrettyPrintExtension.h"
#include "runtime/Array.h"

arrow::Status execution::ExtensionPrinter::PrettyPrint(const arrow::ChunkedArray& chunked_arr, arrow::PrettyPrintOptions options, std::ostream* sink) {
    this->sink = sink;
    this->options = options;
    *(this->sink) << "[";
    for (int i = 0; i < chunked_arr.num_chunks(); ++i) {
        *(this->sink) << "\n[";
        auto chunk = chunked_arr.chunk(i);
        ARROW_RETURN_NOT_OK(arrow::VisitArrayInline(*chunk, this));
        if (i + 1 == chunked_arr.num_chunks()) {
            *(this->sink) << "]";
        } else {
            *(this->sink) << "],";
        }
        
    }
    *(this->sink) << "\n]";
    return arrow::Status::OK();
}

arrow::Status execution::ExtensionPrinter::Visit(const arrow::StringArray& array) {
    int index = 0;
    bool setDots = false;
    for (std::optional<std::string_view> value : array) {
        if (value.has_value() && (index < options.window || index >= (array.length() - options.window))) {
            *sink << "\n";
            std::string str(value.value());
            if (str.length() < runtime::Array::ARRAYHEADER.length()) {
                *sink << str;
            } else {
                std::string header = str.substr(0, runtime::Array::ARRAYHEADER.length());
                if (header == runtime::Array::ARRAYHEADER) {
                    runtime::Array arrayObj(str);
                    *sink << arrayObj.print();
                } else {
                    *sink << str;
                }
            }
            if (index < array.length() - 1) {
               *sink << ",";
            } else {
               *sink << "\n";
            }
        } else if (value.has_value() && !setDots) {
            *sink << "\n";
            *sink << "...";
            setDots = true;
        }
        ++index;
    }
    return arrow::Status::OK();
}

arrow::Status execution::ExtensionPrinter::Visit(const arrow::Array& array) {
    return arrow::Status::NotImplemented("Can not compute sum for array of type ", array.type()->ToString());
}
