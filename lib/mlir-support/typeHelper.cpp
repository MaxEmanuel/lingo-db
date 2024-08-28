#include "mlir-support/typeHelper.h"
#include <stdexcept>

TypeFunctions::ArrayData TypeFunctions::extractArrayDataDB(mlir::OpBuilder& builder, const mlir::Value& array) {
    TypeFunctions::ArrayData result;
    mlir::db::ArrayType data = array.getType().dyn_cast_or_null<mlir::db::ArrayType>();
    if (array.getType().isa<mlir::db::NullableType>()){
        mlir::db::NullableType nullable = array.getType().dyn_cast<mlir::db::NullableType>();
        data = nullable.getType().dyn_cast_or_null<mlir::db::ArrayType>();
    }
    if (!data) {
        throw std::runtime_error("Received value could not be converted to an array type");
    }
    result.type = builder.create<mlir::db::ConstantOp>(builder.getUnknownLoc(), mlir::db::StringType::get(builder.getContext()), builder.getStringAttr(data.getType()));
    result.dimension = builder.create<mlir::db::ConstantOp>(builder.getUnknownLoc(), builder.getI64Type(), builder.getIntegerAttr(builder.getI64Type(), data.getDimensions()));
    result.array = array;
    return result;
}

TypeFunctions::ArrayData TypeFunctions::extractArrayDataUtil(mlir::OpBuilder& builder, const mlir::Value& array) {
    TypeFunctions::ArrayData result;
    mlir::db::ArrayType data = array.getType().dyn_cast_or_null<mlir::db::ArrayType>();
    if (array.getType().isa<mlir::db::NullableType>()){
        mlir::db::NullableType nullable = array.getType().dyn_cast<mlir::db::NullableType>();
        data = nullable.getType().dyn_cast_or_null<mlir::db::ArrayType>();
    }
    if (!data) {
        throw std::runtime_error("Received value could not be converted to an array type");
    }
    result.type = builder.create<mlir::util::CreateConstVarLen>(builder.getUnknownLoc(), mlir::util::VarLen32Type::get(builder.getContext()), builder.getStringAttr(data.getType()));
    result.dimension = builder.create<mlir::arith::ConstantOp>(builder.getUnknownLoc(), builder.getI64Type(), builder.getI64IntegerAttr(data.getDimensions()));
    result.array = array;
    return result;
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

mlir::Type TypeFunctions::getReturnType(const mlir::OpBuilder& builder, const mlir::Value& leftValue, const mlir::Value& rightValue, bool useLeft) {
    if (leftValue.getType().isa<mlir::db::NullableType>() || rightValue.getType().isa<mlir::db::NullableType>()) {
        if (useLeft) {
            if (leftValue.getType().isa<mlir::db::NullableType>()) {
                return leftValue.getType();
            } else {
                return mlir::db::NullableType::get(builder.getContext(), leftValue.getType());
            }
        } else {
            if (rightValue.getType().isa<mlir::db::NullableType>()) {
                return rightValue.getType();
            } else {
                return mlir::db::NullableType::get(builder.getContext(), rightValue.getType());
            }
        }
    }
    if (useLeft) {
        return leftValue.getType();
    }
    return rightValue.getType();
}

mlir::Value TypeFunctions::addCastOp(mlir::OpBuilder& builder, const mlir::Value& toCast, mlir::Type type, bool isNullable) {
    if (isNullable && !type.isa<mlir::db::NullableType>()) {
        auto nullable = toCast.getType().dyn_cast<mlir::db::NullableType>();
        if (!nullable.getType().isa<mlir::NoneType>()){
            type = mlir::db::NullableType::get(builder.getContext(), type);
        }
    }
    bool onlyTargetIsNullable = !isNullable && type.isa<mlir::db::NullableType>();
    if (toCast.getType() == type) { return toCast; }
    if (toCast.getType() == getBaseType(type)) {
      return builder.create<mlir::db::AsNullableOp>(builder.getUnknownLoc(), type, toCast);
    }
    if (onlyTargetIsNullable) {
       mlir::Value casted = builder.create<mlir::db::CastOp>(builder.getUnknownLoc(), getBaseType(type), toCast);
       return builder.create<mlir::db::AsNullableOp>(builder.getUnknownLoc(), type, casted);
    } else {
       return builder.create<mlir::db::CastOp>(builder.getUnknownLoc(), type, toCast);
    }
}

std::tuple<TypeFunctions::ArrayData, TypeFunctions::ArrayData> TypeFunctions::castToArrayIfNecessary(mlir::OpBuilder& builder, const mlir::Value& left, const mlir::Value& right) {
    TypeFunctions::ArrayData array1, array2;
    mlir::Type leftType = getBaseType(left.getType());
    mlir::Type rightType = getBaseType(right.getType());
    bool leftIsNullable = left.getType().isa<mlir::db::NullableType>();
    bool rightIsNullable = right.getType().isa<mlir::db::NullableType>();
    if (!leftType.isa<mlir::db::ArrayType>() && !rightType.isa<mlir::db::ArrayType>()) {
        throw std::runtime_error("Both given arrays are not of type array");
    }
    // If left value is not an array, cast it to an array with metadata of right array.
    if (!leftType.isa<mlir::db::ArrayType>()) {
       array1 = TypeFunctions::extractArrayDataDB(builder, right);
       array1.array = TypeFunctions::addCastOp(builder, left, rightType, leftIsNullable);
    } else {
       array1 = TypeFunctions::extractArrayDataDB(builder, left);
    }
    // If right value is not an array, cast it to an array with metadata of left array.
    if (!rightType.isa<mlir::db::ArrayType>()) {
       array2 = TypeFunctions::extractArrayDataDB(builder, left);
       array2.array = TypeFunctions::addCastOp(builder, right, leftType, rightIsNullable); 
    } else {
       array2 = TypeFunctions::extractArrayDataDB(builder, right);
    }
    return std::make_tuple(array1, array2);
}

TypeFunctions::ArrayData TypeFunctions::castToArrayIfNecessary(mlir::OpBuilder& builder, const mlir::Value& value, const mlir::Type& type) {
    if (getBaseType(value.getType()).isa<mlir::db::ArrayType>()) {
        return TypeFunctions::extractArrayDataDB(builder, value);
    }
    bool isNullable = value.getType().isa<mlir::db::NullableType>();
    TypeFunctions::ArrayData result;
    // Make assumption of the dimension
    auto dimension = 1;
    std::string arrayType = "";

    mlir::Type baseType = getBaseType(type);
    // Check which type the value has
    if (auto integer = baseType.dyn_cast<mlir::IntegerType>()){
        if (integer.getWidth() < 64) {
            arrayType = "int32[]";
        } else {
            arrayType = "int64[]";
        }
    } else if (auto floatType = baseType.dyn_cast<mlir::FloatType>()) {
        if (floatType.getWidth() < 64) {
            arrayType = "float[]";
        } else {
            arrayType = "double[]";
        }
    } else if (baseType.isa<mlir::db::DecimalType>()) {
        arrayType = "double[]";
    } else if (baseType.isa<mlir::db::CharType>() || baseType.isa<mlir::db::StringType>() || baseType.isa<mlir::NoneType>()){
        arrayType = "string[]";
    } else {
        throw std::runtime_error("The entered type cannot be used to create an array");
    }
    result.array = TypeFunctions::addCastOp(builder, value, mlir::db::ArrayType::get(builder.getContext(), dimension, arrayType), isNullable);
    result.dimension = builder.create<mlir::arith::ConstantOp>(builder.getUnknownLoc(), builder.getI64Type(), builder.getI64IntegerAttr(dimension));
    result.type = builder.create<mlir::db::ConstantOp>(builder.getUnknownLoc(), mlir::db::StringType::get(builder.getContext()), builder.getStringAttr(arrayType));
    return result;
}