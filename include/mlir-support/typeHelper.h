#include <tuple>
#include "mlir/Dialect/DB/IR/DBDialect.h"
#include "mlir/Dialect/DB/IR/DBOps.h"
namespace TypeFunctions
{
    /**
     * This function extracts all necessary information from the ```mlir::db::ArrayType```and creates corresponding 
     * ```mlir::Value``` objects, which are stored inside a ```std::tuple```. The first value stores the dimension
     * of the array whereas the second element stores the type of the array elements.
     * @param builder           A reference to a ```mlir::OpBuilder```. This object allows to create ```mlir``` 
     *                          operations as well as types and values
     * @param array             A reference to the ```mlir::db::ArrayType```which information should be extracted.
     *                          Can also be nested inside a ```mlir::db::NullableType```
     */
    std::tuple<mlir::Value, mlir::Value> extractArrayData(mlir::OpBuilder& builder, const mlir::Value& array);
} // namespace TypeFunctions