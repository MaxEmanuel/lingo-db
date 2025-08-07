add_library(runtime OBJECT
        RecordBatchInfo.cpp
        ThreadLocal.cpp
        Heap.cpp
        TableBuilder.cpp
        DumpRuntime.cpp
        DataSourceIteration.cpp
        GrowingBuffer.cpp
        Buffer.cpp
        SimpleState.cpp
        LazyJoinHashtable.cpp
        SegmentTreeView.cpp
        Hashtable.cpp
        PreAggregationHashtable.cpp
        HashMultiMap.cpp
        StringRuntime.cpp
        FloatRuntime.cpp
        DecimalRuntime.cpp
        IntegerRuntime.cpp
        Hash.cpp
        Timing.cpp
        DateRuntime.cpp
        ExecutionContext.cpp
        RelationHelper.cpp
        #Database.cpp
        MetaData.cpp
        #ArrowDirDatabase.cpp
        #ExternalArrowDatabase.cpp
        #MetaDataOnlyDatabase.cpp
        #ExternalHashIndex.cpp
        HashIndex.cpp
        Relation.cpp
        Session.cpp
        Catalog.cpp
        Array/Array.cpp
        Array/ArrayElementHandling.cpp
        Array/ArrayWidthHandling.cpp
        Array/ArrayNullHandling.cpp
        Array/ArrayParsing.cpp
        Array/ArrayAppend.cpp
        Array/ArraySlice.cpp
        Array/ArraySubscript.cpp
        Array/ArrayArithmetic.cpp
        Array/ArrayActivation.cpp
        Array/ArrayTranspose.cpp
        Array/ArrayFill.cpp
        Array/ArrayCast.cpp
        Array/ArrayPrint.cpp
        ArrayRuntime.cpp)
find_library(OPENBLAS_LIB NAMES openblas HINTS "${PROJECT_SOURCE_DIR}/vendored/openblas/lib")

if(NOT OPENBLAS_LIB)
  message(FATAL_ERROR "OpenBLAS library not found. Please check the path.")
endif()
target_link_libraries(runtime PRIVATE tbb arrow ${OPENBLAS_LIB})

