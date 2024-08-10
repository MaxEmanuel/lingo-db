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
    mlir::Value arrayDim = builder.create<mlir::db::ConstantOp>(builder.getUnknownLoc(), builder.getI64Type(), builder.getIntegerAttr(builder.getI64Type(), data.getDimensions()));
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
    mlir::Value arrayDim = builder.create<mlir::arith::ConstantOp>(builder.getUnknownLoc(), builder.getI64Type(), builder.getI64IntegerAttr(data.getDimensions()));
    return std::make_tuple(arrayDim, arrayType);
}

std::tuple<int, std::string> TypeFunctions::extractArrayRawData(mlir::OpBuilder& builder, const mlir::Value& array) {
    mlir::db::ArrayType data = array.getType().dyn_cast_or_null<mlir::db::ArrayType>();
    if (array.getType().isa<mlir::db::NullableType>()){
        mlir::db::NullableType nullable = array.getType().dyn_cast<mlir::db::NullableType>();
        data = nullable.getType().dyn_cast_or_null<mlir::db::ArrayType>();
    }
    if (!data) {
        throw std::runtime_error("Received value could not be converted to an array type");
    }
    return std::make_tuple(data.getDimensions(), data.getType());
}

mlir::Type TypeFunctions::getReturnType(const mlir::Value& leftValue, const mlir::Value& rightValue) {
    auto typeId = mlir::TypeID::get<mlir::db::ConstantOp>();
    if (leftValue.getDefiningOp()->getName().getTypeID() == typeId) {
        return rightValue.getType();
    }
    return leftValue.getType();
}