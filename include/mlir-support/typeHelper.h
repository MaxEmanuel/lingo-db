#include <tuple>
#include "mlir/Dialect/DB/IR/DBDialect.h"
#include "mlir/Dialect/DB/IR/DBOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/util/UtilOps.h"

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
     * @note                    If the given ```array``` does not include an ```mlir::db::ArrayType``` it will 
     *                          throw an ```std::runtime_error```.
     * @note                    This function can be used within the DB-Dialect
     */
    std::tuple<mlir::Value, mlir::Value> extractArrayDataDB(mlir::OpBuilder& builder, const mlir::Value& array);

    /**
     * This function extracts all necessary information from the ```mlir::db::ArrayType```and creates corresponding 
     * ```mlir::Value``` objects, which are stored inside a ```std::tuple```. The first value stores the dimension
     * of the array whereas the second element stores the type of the array elements.
     * @param builder           A reference to a ```mlir::OpBuilder```. This object allows to create ```mlir``` 
     *                          operations as well as types and values
     * @param array             A reference to the ```mlir::db::ArrayType```which information should be extracted.
     *                          Can also be nested inside a ```mlir::db::NullableType```
     * @note                    If the given ```array``` does not include an ```mlir::db::ArrayType``` it will 
     *                          throw an ```std::runtime_error```.
     * @note                    This function can be used within the Util-Dialect
     */
    std::tuple<mlir::Value, mlir::Value> extractArrayDataUtil(mlir::OpBuilder& builder, const mlir::Value& array);

    /**
     * This function extracts all necessary information from the ```mlir::db::ArrayType```and returns the raw 
     * values, which are stored inside a ```std::tuple```. The first value stores the dimension
     * of the array whereas the second element stores the type of the array elements.
     * @param builder           A reference to a ```mlir::OpBuilder```. This object allows to create ```mlir``` 
     *                          operations as well as types and values
     * @param array             A reference to the ```mlir::db::ArrayType```which information should be extracted.
     *                          Can also be nested inside a ```mlir::db::NullableType```
     * @note                    If the given ```array``` does not include an ```mlir::db::ArrayType``` it will 
     *                          throw an ```std::runtime_error```.
     */
    std::tuple<int, std::string> extractArrayRawData(mlir::OpBuilder& builder, const mlir::Value& array);

    /**
     * This function returns a ```mlir::Type``` which represents the return-type of a runtime function. Its main
     * goal is to choose the type specification which is defined in a table (or simpler, choose that type which 
     * is not inside a ```mlir::db::ConstantOp``` - This can be nullable and possibly leads to an error).
     * @param leftValue         A reference to the left ```mlir::Value``` of the operation.
     * @param rightValue        A reference to the right ```mlir::Value``` of the operation.
     * @return                  The return-type of the runtime function.
     */
    mlir::Type getReturnType(const mlir::Value& leftValue, const mlir::Value& rightValue);
}