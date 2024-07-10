#include "mlir-support/typeHelper.h"
#include <stdexcept>

std::tuple<mlir::Value, mlir::Value> TypeFunctions::extractArrayDataDB(mlir::OpBuilder& builder, const mlir::Value& array) {

    mlir::db::ArrayType data = array.getType().dyn_cast_or_null<mlir::db::ArrayType>();
    if (array.getType().isa<mlir::db::NullableType>()){
        mlir::db::NullableType nullable = array.getType().dyn_cast<mlir::db::NullableType>();
        data = nullable.getType().dyn_cast_or_null<mlir::db::ArrayType>();
    }
    if (!data) {
        throw std::runtime_error("Received value could not be converted to an array type");
    }
    mlir::Value arrayType = builder.create<mlir::db::ConstantOp>(builder.getUnknownLoc(), mlir::db::StringType::get(builder.getContext()), builder.getStringAttr(data.getType()));
    mlir::Value arrayDim = builder.create<mlir::db::ConstantOp>(builder.getUnknownLoc(), builder.getI32Type(), builder.getIntegerAttr(builder.getI32Type(), data.getDimensions()));
    return std::make_tuple(arrayDim, arrayType);
}

std::tuple<mlir::Value, mlir::Value> TypeFunctions::extractArrayDataUtil(mlir::OpBuilder& builder, const mlir::Value& array) {
    mlir::db::ArrayType data = array.getType().dyn_cast_or_null<mlir::db::ArrayType>();
    if (array.getType().isa<mlir::db::NullableType>()){
        mlir::db::NullableType nullable = array.getType().dyn_cast<mlir::db::NullableType>();
        data = nullable.getType().dyn_cast_or_null<mlir::db::ArrayType>();
    }
    if (!data) {
        throw std::runtime_error("Received value could not be converted to an array type");
    }
    mlir::Value arrayType = builder.create<mlir::util::CreateConstVarLen>(builder.getUnknownLoc(), mlir::util::VarLen32Type::get(builder.getContext()), builder.getStringAttr(data.getType()));
    mlir::Value arrayDim = builder.create<mlir::arith::ConstantOp>(builder.getUnknownLoc(), builder.getI32Type(), builder.getI32IntegerAttr(data.getDimensions()));
    return std::make_tuple(arrayDim, arrayType);
}