#include <tuple>
#include "mlir/Dialect/DB/IR/DBDialect.h"
#include "mlir/Dialect/DB/IR/DBOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/util/UtilOps.h"

namespace TypeFunctions
{
    struct ArrayData {
        mlir::Value array;
        mlir::Value dimension;
        mlir::Value type;
    };

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
    ArrayData extractArrayDataDB(mlir::OpBuilder& builder, const mlir::Value& array);

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
    ArrayData extractArrayDataUtil(mlir::OpBuilder& builder, const mlir::Value& array);

    /**
     * This function extracts all necessary information from the ```mlir::db::ArrayType``` and returns the raw 
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
     * This function returns a ```mlir::Type``` which represents the return-type of a runtime function. It mainly looks
     * if some ```mlir::db::NullableType``` is involved. If it is the case, it will either return the original type of 
     * a ```mlir::Value``` or wrap it up inside a ```mlir::db::NullableType```. Moreover by using the ```useLeft``` parameter,
     * a decision is made which  ```mlir::Value``` (```leftValue``` or ```rightValue```) should be used for that.
     * @param builder           A reference to a ```mlir::OpBuilder```. This object allows to create ```mlir``` 
     *                          operations as well as types and values
     * @param leftValue         A reference to the left ```mlir::Value``` of the operation.
     * @param rightValue        A reference to the right ```mlir::Value``` of the operation.
     * @param useLeft           Optional - Should be the type of ```leftValue``` or ```rightValue``` be returned. The default 
     *                          value is ```true```
     * @return                  The return-type of the runtime function.
     */
    mlir::Type getReturnType(const mlir::OpBuilder& builder, const mlir::Value& leftValue, const mlir::Value& rightValue, bool useLeft = true);

    /**
     * This function creates a ```mlir::db::CastOp``` upon the given ```toCast``` value. By executing this ```mlir::db::CastOp```, the
     * database tries to cast the given value to the specified ```mlir::Type```.
     * @param builder           A reference to a ```mlir::OpBuilder```. This object allows to create ```mlir``` 
     *                          operations as well as types and values
     * @param toCast            A reference to the ```mlir::Value``` which should be casted
     * @param type              The resulting ```mlir::Type``` generated by the ```mlir::db::CastOp```.
     * @return                  An updated version of the given ```mlir::Value``` wrapped inside a ```mlir::db::CastOp```.
     */
    mlir::Value addCastOp(mlir::OpBuilder& builder, const mlir::Value& toCast, mlir::Type type);

    /**
     * This function cast a non ```mlir::db::ArrayType``` to this type using the specifications of the other parameter which needs to be 
     * a ```mlir::db::ArrayType```. This means e.g. if ```left``` is not an ```mlir::db::ArrayType```, it will be casted to that with the 
     * specification of ```right``` and vice versa. If both are ```mlir::db::ArrayType``` nothing will happen.
     * @param builder           A reference to a ```mlir::OpBuilder```. This object allows to create ```mlir``` 
     *                          operations as well as types and values
     * @param left              The left ```mlir::Value``` of an array operation
     * @param right             The right ```mlir::Value``` of an array operation
     * @return                  A tuple of two ```ArrayData``` structures which includes all necessary ```mlir::Value``` objects of the 
     *                          ```left``` and ```right``` array.
     * @note                    If both parameters are not a ```mlir::db::ArrayType``` an ```std::runtime_error``` will be thrown.
     */
    std::tuple<ArrayData, ArrayData> castToArrayIfNecessary(mlir::OpBuilder& builder, const mlir::Value& left, const mlir::Value& right);

    /**
     * This function cast a non ```mlir::db::ArrayType``` to this type using some asssumptions according its specification. Those assumptions
     * are that the dimension of that array will be 1 and the type will be derivated according the following rules: Single numeric value (e.g. 1 or 4.22)
     * will be interpreted as an array of their coresponding type (here int32[] and float[]). Any other kind of input is interpreted as an array with 
     * string values.
     * @param builder           A reference to a ```mlir::OpBuilder```. This object allows to create ```mlir``` 
     *                          operations as well as types and values
     * @param value             The ```mlir::Value``` which should be casted
     * @return                  An ```ArrayData``` structure which includes all necessary ```mlir::Value``` objects of the 
     *                          resulting array.
     * @note                    If the original type can be converted it will thrown an ```std::runtime_error```.
     */
    ArrayData castToArrayIfNecessary(mlir::OpBuilder& builder, const mlir::Value& value);
}