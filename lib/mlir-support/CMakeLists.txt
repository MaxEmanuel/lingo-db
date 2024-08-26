add_library(mlir-support OBJECT parsing.cpp eval.cpp tostring.cpp typeHelper.cpp)
target_link_libraries(mlir-support PRIVATE arrow)
