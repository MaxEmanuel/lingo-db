// RUN: run-mlir %s | FileCheck %s
module {
    func.func @main ()  {
        // TODO: Currently in string array prints random space in every element despite first element (Unkown source)
        
        // VARIABLES
        
        %intArray1 = db.constant ( "{1,2,3}" ) : !db.array<1,"int32[]">
        %intArray2 = db.constant ( "{{1,2,3},{4,5,6}}" ) : !db.array<2,"int32[]">
        %intArray3 = db.constant ( "{{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}}" ) : !db.array<3,"int32[]">

        %floatArray1 = db.constant ( "{1.1,2.2,3.3}" ) : !db.array<1,"float[]">
        %floatArray2 = db.constant ( "{{1.1,2.2,3.3},{4.4,5.5,6.6}}" ) : !db.array<2,"float[]">
        %floatArray3 = db.constant ( "{{{1.1,2.2,3.3,4.4},{5.5,6.6,7.7,8.8},{9.9,10.1,11.2,12.3}},{{1.1,2.2,3.3,4.4},{5.5,6.6,7.7,8.8},{9.9,10.1,11.2,12.3}}}" ) : !db.array<3,"float[]">

        %stringArray1 = db.constant ( "{'earth', 'moon', 'mars'}" ) : !db.array<1,"string[]">
        %stringArray2 = db.constant ( "{{'earth', 'moon', 'mars'},{'jupiter', 'saturn', 'neptun'}}" ) : !db.array<2,"string[]">
        %stringArray3 = db.constant ( "{{{'earth', 'moon', 'mars'},{'jupiter', 'saturn', 'neptun'}},{{'earth', 'moon', 'mars'},{'jupiter', 'saturn', 'neptun'}}}" ) : !db.array<3,"string[]">

        %intArray2Null1 = db.constant ( "{{1,2,3},Null}" ) : !db.array<2,"int32[]">
        %intArray2Null2 = db.constant ( "{{1,2,3},{4,Null,6}}" ) : !db.array<2,"int32[]">
        
        %intType = db.constant ("int32[]") : !db.string
        %floatType = db.constant ("float[]") : !db.string
        %stringType = db.constant ("string[]") : !db.string
        
        %dim1 = db.constant (1) : i64
        %dim2 = db.constant (2) : i64
        %dim3 = db.constant (3) : i64

        // BE AWARE: {{[{]}} is used to prevent the test environment to interpret {{}} as regular expression field

        // CONCAT

        // {1,2,3} || {1,2,3} -> {1, 2, 3, 1, 2, 3}

        //CHECK: array("{1, 2, 3, 1, 2, 3}")
        %oneDim_1 = db.runtime_call "ConcatenateArray" (%intArray1, %dim1, %intType, %intArray1, %dim1, %intType) : (!db.array<1,"int32[]">, i64, !db.string, !db.array<1,"int32[]">, i64, !db.string) -> !db.array<1,"int32[]">
        db.runtime_call "DumpValue" (%oneDim_1) : (!db.array<1,"int32[]">) -> ()

        // {{1.1,2.2,3.3},{4.4,5.5,6.6}} || {{1.1,2.2,3.3},{4.4,5.5,6.6}} -> {{1.1,2.2,3.3},{4.4,5.5,6.6}, {1.1,2.2,3.3},{4.4,5.5,6.6}}

        //CHECK: array("{{[{]}}{1.1, 2.2, 3.3}, {4.4, 5.5, 6.6}, {1.1, 2.2, 3.3}, {4.4, 5.5, 6.6}}")
        %twoDim_2 = db.runtime_call "ConcatenateArray" (%floatArray2, %dim2, %floatType, %floatArray2, %dim2, %floatType) : (!db.array<2,"float[]">, i64, !db.string, !db.array<2,"float[]">, i64, !db.string) -> !db.array<2,"float[]">
        db.runtime_call "DumpValue" (%twoDim_2) : (!db.array<2,"float[]">) -> ()

        // {{{'earth', 'moon', 'mars'},{'jupiter', 'saturn', 'neptun'}},{{'earth', 'moon', 'mars'},{'jupiter', 'saturn', 'neptun'}}} || {{{'earth', 'moon', 'mars'},{'jupiter', 'saturn', 'neptun'}},{{'earth', 'moon', 'mars'},{'jupiter', 'saturn', 'neptun'}}}
        // -> {{{'earth', 'moon', 'mars'},{'jupiter', 'saturn', 'neptun'}},{{'earth', 'moon', 'mars'},{'jupiter', 'saturn', 'neptun'}}, {{'earth', 'moon', 'mars'},{'jupiter', 'saturn', 'neptun'}},{{'earth', 'moon', 'mars'},{'jupiter', 'saturn', 'neptun'}}}

        //CHECK: array("{{[{]}}{{[{]}}{"earth", " moon", " mars"}, {"jupiter", " saturn", " neptun"}}, {{[{]}}{"earth", " moon", " mars"}, {"jupiter", " saturn", " neptun"}}, {{[{]}}{"earth", " moon", " mars"}, {"jupiter", " saturn", " neptun"}}, {{[{]}}{"earth", " moon", " mars"}, {"jupiter", " saturn", " neptun"}}}")
        %threeDim_3 = db.runtime_call "ConcatenateArray" (%stringArray3, %dim3, %stringType, %stringArray3, %dim3, %stringType) : (!db.array<3,"string[]">, i64, !db.string, !db.array<3,"string[]">, i64, !db.string) -> !db.array<3,"string[]">
        db.runtime_call "DumpValue" (%threeDim_3) : (!db.array<3,"string[]">) -> ()

        // {{1,2,3},{4,5,6}} || {1,2,3} -> {{1, 2, 3}, {4, 5, 6}, {1, 2, 3}}

        //CHECK: array("{{[{]}}{1, 2, 3}, {4, 5, 6}, {1, 2, 3}}")
        %OneToTwoDim_4 = db.runtime_call "ConcatenateArray" (%intArray2, %dim2, %intType, %intArray1, %dim1, %intType) : (!db.array<2,"int32[]">, i64, !db.string, !db.array<1,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%OneToTwoDim_4) : (!db.array<2,"int32[]">) -> ()

        // {{1,2,3},{4,5,6}} || {{1,2,3},Null} -> {{1, 2, 3}, {4, 5, 6}, {1, 2, 3}, null}

        //CHECK: array("{{[{]}}{1, 2, 3}, {4, 5, 6}, {1, 2, 3}, null}")
        %NullElem_5 = db.runtime_call "ConcatenateArray" (%intArray2, %dim2, %intType, %intArray2Null1, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string, !db.array<2,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%NullElem_5) : (!db.array<2,"int32[]">) -> ()

        // {{1,2,3},{4,5,6}} || {{1,2,3},{4,Null,6}} -> {{1, 2, 3}, {4, 5, 6}, {1, 2, 3}, {4, null, 6}}

        //CHECK: array("{{[{]}}{1, 2, 3}, {4, 5, 6}, {1, 2, 3}, {4, null, 6}}")
        %NullValue_6 = db.runtime_call "ConcatenateArray" (%intArray2, %dim2, %intType, %intArray2Null2, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string, !db.array<2,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%NullValue_6) : (!db.array<2,"int32[]">) -> ()

        // SLICE

        %start = db.constant (1) : i64
        %stop = db.constant (2) : i64
        %subDim1 = db.constant (1) : i64
        %subDim2 = db.constant (2) : i64
        %subDim3 = db.constant (3) : i64

        // {1.1,2.2,3.3}[1:2] -> {1.1, 2.2}

        //CHECK: array("{1.1, 2.2}")
        %oneDim_7 = db.runtime_call "ArrayRange" (%floatArray1, %dim1, %floatType, %start, %stop, %subDim1) : (!db.array<1,"float[]">, i64, !db.string, i64, i64, i64) -> !db.array<1,"float[]">
        db.runtime_call "DumpValue" (%oneDim_7) : (!db.array<1,"float[]">) -> ()

        // {{'earth', 'moon', 'mars'},{'jupiter', 'saturn', 'neptun'}}[1:2] -> {{"earth", "moon", "mars"}, {"jupiter", "saturn", "neptun"}}

        //CHECK: array("{{[{]}}{"earth", " moon", " mars"}, {"jupiter", " saturn", " neptun"}}")
        %twoDim_8 = db.runtime_call "ArrayRange" (%stringArray2, %dim2, %stringType, %start, %stop, %subDim2) : (!db.array<2,"string[]">, i64, !db.string, i64, i64, i64) -> !db.array<2,"string[]">
        db.runtime_call "DumpValue" (%twoDim_8) : (!db.array<2,"string[]">) -> ()

        // {{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}}[1:2] -> {{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}}

        //CHECK: array("{{[{]}}{{[{]}}{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}}, {{[{]}}{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}}}")
        %threeDim_9 = db.runtime_call "ArrayRange" (%intArray3, %dim3, %intType, %start, %stop, %subDim3) : (!db.array<3,"int32[]">, i64, !db.string, i64, i64, i64) -> !db.array<3,"int32[]">
        db.runtime_call "DumpValue" (%threeDim_9) : (!db.array<3,"int32[]">) -> ()
        
        %start2 = db.constant (2) : i64
        %stop2 = db.constant (2) : i64
        %stop3 = db.constant (3) : i64

        // {{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}}[2:2][2:3][2:3] -> {{{6,7},{10,11}}}

        //CHECK: array("{{[{]}}{{[{]}}{6, 7}, {10, 11}}}")
        %slice_1 = db.runtime_call "ArrayRange" (%intArray3, %dim3, %intType, %start2, %stop2, %subDim3) : (!db.array<3,"int32[]">, i64, !db.string, i64, i64, i64) -> !db.array<3,"int32[]">
        %slice_2 = db.runtime_call "ArrayRange" (%slice_1, %dim3, %intType, %start2, %stop3, %subDim2) : (!db.array<3,"int32[]">, i64, !db.string, i64, i64, i64) -> !db.array<3,"int32[]">
        %slice_3 = db.runtime_call "ArrayRange" (%slice_2, %dim3, %intType, %start2, %stop3, %subDim1) : (!db.array<3,"int32[]">, i64, !db.string, i64, i64, i64) -> !db.array<3,"int32[]">
        db.runtime_call "DumpValue" (%slice_3) : (!db.array<3,"int32[]">) -> ()

        // {{1,2,3},Null}[2:2] -> {null}

        //CHECK: array("{null}")
        %NullElem_11 = db.runtime_call "ArrayRange" (%intArray2Null1, %dim2, %intType, %start2, %stop, %subDim2) : (!db.array<2,"int32[]">, i64, !db.string, i64, i64, i64) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%NullElem_11) : (!db.array<2,"int32[]">) -> ()

        // SUBSCRIPT

        %index1 = db.constant (1) : i64
        %index2 = db.constant (2) : i64
        %index3 = db.constant (3) : i64

        // {1.1,2.2,3.3}[2] -> 2.2

        //CHECK: array("2.2")
        %oneDim_12 = db.runtime_call "ArrayElement" (%floatArray1, %dim1, %floatType, %index2) : (!db.array<1,"float[]">, i64, !db.string, i64) -> !db.array<1,"float[]">
        db.runtime_call "DumpValue" (%oneDim_12) : (!db.array<1,"float[]">) -> ()

        // {{'earth', 'moon', 'mars'},{'jupiter', 'saturn', 'neptun'}}[1] -> {"earth", "moon", "mars"}

        //CHECK: array("{"earth", " moon", " mars"}")
        %twoDim_13 = db.runtime_call "ArrayElement" (%stringArray2, %dim2, %stringType, %index1) : (!db.array<2,"string[]">, i64, !db.string, i64) -> !db.array<2,"string[]">
        db.runtime_call "DumpValue" (%twoDim_13) : (!db.array<2,"string[]">) -> ()

        // {{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}}[2] -> {{1,2,3,4},{5,6,7,8},{9,10,11,12}}

        //CHECK: array("{{[{]}}{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}}")
        %threeDim_14 = db.runtime_call "ArrayElement" (%intArray3, %dim3, %intType, %index2) : (!db.array<3,"int32[]">, i64, !db.string, i64) -> !db.array<3,"int32[]">
        db.runtime_call "DumpValue" (%threeDim_14) : (!db.array<3,"int32[]">) -> ()

        // {{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}}[1][2][3] -> 7

        //CHECK: array("7")
        %sub_1 = db.runtime_call "ArrayElement" (%intArray3, %dim3, %intType, %index1) : (!db.array<3,"int32[]">, i64, !db.string, i64) -> !db.array<3,"int32[]">
        %sub_2 = db.runtime_call "ArrayElement" (%sub_1, %dim2, %intType, %index2) : (!db.array<3,"int32[]">, i64, !db.string, i64) -> !db.array<3,"int32[]">
        %sub_3 = db.runtime_call "ArrayElement" (%sub_2, %dim1, %intType, %index3) : (!db.array<3,"int32[]">, i64, !db.string, i64) -> !db.array<3,"int32[]">
        db.runtime_call "DumpValue" (%sub_3) : (!db.array<3,"int32[]">) -> ()

        // GET_DIMENSIONS

        // {1.1,2.2,3.3} -> [1:3]

        //CHECK: string("[1:3]")
        %oneDim_15 = db.runtime_call "ArrayDimensions" (%floatArray1, %dim1, %floatType) : (!db.array<1,"float[]">, i64, !db.string) -> !db.string
        db.runtime_call "DumpValue" (%oneDim_15) : (!db.string) -> ()

        // {{'earth', 'moon', 'mars'},{'jupiter', 'saturn', 'neptun'}} -> [1:2][1:3]

        //CHECK: string("[1:2][1:3]")
        %twoDim_16 = db.runtime_call "ArrayDimensions" (%stringArray2, %dim2, %stringType) : (!db.array<2,"string[]">, i64, !db.string) -> !db.string
        db.runtime_call "DumpValue" (%twoDim_16) : (!db.string) -> ()

        // {{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}} -> [1:2][1:3][1:4]

        //CHECK: string("[1:2][1:3][1:4]")
        %threeDim_17 = db.runtime_call "ArrayDimensions" (%intArray3, %dim3, %intType) : (!db.array<3,"int32[]">, i64, !db.string) -> !db.string
        db.runtime_call "DumpValue" (%threeDim_17) : (!db.string) -> ()

        %arrayDim = db.constant ( "{{1,2},{4,5,6},{3,3,3,3}}" ) : !db.array<2,"int32[]">

        //CHECK: string("[1:3][1:2]")
        %unequalElems_18 = db.runtime_call "ArrayDimensions" (%arrayDim, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string) -> !db.string
        db.runtime_call "DumpValue" (%unequalElems_18) : (!db.string) -> ()


        // CARDINALITY

        // {1.1,2.2,3.3} -> 3

        //CHECK: int(3)
        %oneDim_19 = db.runtime_call "ArrayCardinality" (%floatArray1, %dim1, %floatType) : (!db.array<1,"float[]">, i64, !db.string) -> i64
        db.runtime_call "DumpValue" (%oneDim_19) : (i64) -> ()

        // {{'earth', 'moon', 'mars'},{'jupiter', 'saturn', 'neptun'}} -> 6

        //CHECK: int(6)
        %twoDim_20 = db.runtime_call "ArrayCardinality" (%stringArray2, %dim2, %stringType) : (!db.array<2,"string[]">, i64, !db.string) -> i64
        db.runtime_call "DumpValue" (%twoDim_20) : (i64) -> ()

        // {{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}} -> 24

        //CHECK: int(24)
        %threeDim_21 = db.runtime_call "ArrayCardinality" (%intArray3, %dim3, %intType) : (!db.array<3,"int32[]">, i64, !db.string) -> i64
        db.runtime_call "DumpValue" (%threeDim_21) : (i64) -> ()

        // {{1,2,3},Null} -> 4

        //CHECK: int(4)
        %NullElem_22 = db.runtime_call "ArrayCardinality" (%intArray2Null1, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string) -> i64
        db.runtime_call "DumpValue" (%NullElem_22) : (i64) -> ()

        // "{{1,2,3},{4,Null,6}}" -> 6

        //CHECK: int(6)
        %NullValue_23 = db.runtime_call "ArrayCardinality" (%intArray2Null2, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string) -> i64
        db.runtime_call "DumpValue" (%NullValue_23) : (i64) -> ()

        // ADD

        // {1.1,2.2,3.3} + {1.1,2.2,3.3} -> {2.2,4.4,6.6}

        //CHECK: array("{2.2, 4.4, 6.6}")
        %oneDim_24 = db.runtime_call "ArrayAdd" (%floatArray1, %dim1, %floatType, %floatArray1, %dim1, %floatType) : (!db.array<1,"float[]">, i64, !db.string, !db.array<1,"float[]">, i64, !db.string) -> !db.array<1,"float[]">
        db.runtime_call "DumpValue" (%oneDim_24) : (!db.array<1,"float[]">) -> ()

        // {{1,2,3},{4,5,6}} + {{1,2,3},{4,5,6}} -> {{2,4,6},{8,10,12}}

        //CHECK: array("{{[{]}}{2, 4, 6}, {8, 10, 12}}")
        %twoDim_25 = db.runtime_call "ArrayAdd" (%intArray2, %dim2, %intType, %intArray2, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string, !db.array<2,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%twoDim_25) : (!db.array<2,"int32[]">) -> ()

        // {{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}} + {{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}} 
        // -> {{{2, 4, 6, 8}, {10, 12, 14, 16}, {18, 20, 22, 24}}, {{2, 4, 6, 8}, {10, 12, 14, 16}, {18, 20, 22, 24}}}

        //CHECK: array("{{[{]}}{{[{]}}{2, 4, 6, 8}, {10, 12, 14, 16}, {18, 20, 22, 24}}, {{[{]}}{2, 4, 6, 8}, {10, 12, 14, 16}, {18, 20, 22, 24}}}")
        %threeDim_26 = db.runtime_call "ArrayAdd" (%intArray3, %dim3, %intType, %intArray3, %dim3, %intType) : (!db.array<3,"int32[]">, i64, !db.string, !db.array<3,"int32[]">, i64, !db.string) -> !db.array<3,"int32[]">
        db.runtime_call "DumpValue" (%threeDim_26) : (!db.array<3,"int32[]">) -> ()

        // {{1,2,3},{4,5,6}} + {{1,2,3},Null} -> {{2,4,6},null}

        //CHECK: array("{{[{]}}{2, 4, 6}, null}")
        %NullElem_27 = db.runtime_call "ArrayAdd" (%intArray2, %dim2, %intType, %intArray2Null1, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string, !db.array<2,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%NullElem_27) : (!db.array<2,"int32[]">) -> ()

        // {{1,2,3},{4,5,6}} + {{1,2,3},{4,Null,6}} -> {{2,4,6},{8,null,12}}

        //CHECK: array("{{[{]}}{2, 4, 6}, {8, null, 12}}")
        %NullValue_28 = db.runtime_call "ArrayAdd" (%intArray2, %dim2, %intType, %intArray2Null2, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string, !db.array<2,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%NullValue_28) : (!db.array<2,"int32[]">) -> ()

        // SUB

        // {1.1,2.2,3.3} - {1.1,2.2,3.3} -> {0,0,0}

        //CHECK: array("{0, 0, 0}")
        %oneDim_29 = db.runtime_call "ArraySub" (%floatArray1, %dim1, %floatType, %floatArray1, %dim1, %floatType) : (!db.array<1,"float[]">, i64, !db.string, !db.array<1,"float[]">, i64, !db.string) -> !db.array<1,"float[]">
        db.runtime_call "DumpValue" (%oneDim_29) : (!db.array<1,"float[]">) -> ()

        // {{1,2,3},{4,5,6}} - {{1,2,3},{4,5,6}} -> {{0,0,0},{0,0,0}}

        //CHECK: array("{{[{]}}{0, 0, 0}, {0, 0, 0}}")
        %twoDim_30 = db.runtime_call "ArraySub" (%intArray2, %dim2, %intType, %intArray2, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string, !db.array<2,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%twoDim_30) : (!db.array<2,"int32[]">) -> ()

        // {{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}} - {{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}} 
        // -> {{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}}

        //CHECK: array("{{[{]}}{{[{]}}{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, {{[{]}}{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}}")
        %threeDim_31 = db.runtime_call "ArraySub" (%intArray3, %dim3, %intType, %intArray3, %dim3, %intType) : (!db.array<3,"int32[]">, i64, !db.string, !db.array<3,"int32[]">, i64, !db.string) -> !db.array<3,"int32[]">
        db.runtime_call "DumpValue" (%threeDim_31) : (!db.array<3,"int32[]">) -> ()

        // {{1,2,3},{4,5,6}} - {{1,2,3},Null} -> {{0,0,0},null}

        //CHECK: array("{{[{]}}{0, 0, 0}, null}")
        %NullElem_32 = db.runtime_call "ArraySub" (%intArray2, %dim2, %intType, %intArray2Null1, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string, !db.array<2,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%NullElem_32) : (!db.array<2,"int32[]">) -> ()

        // {{1,2,3},{4,5,6}} - {{1,2,3},{4,Null,6}} -> {{0,0,0},{0,null,0}}

        //CHECK: array("{{[{]}}{0, 0, 0}, {0, null, 0}}")
        %NullValue_33 = db.runtime_call "ArraySub" (%intArray2, %dim2, %intType, %intArray2Null2, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string, !db.array<2,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%NullValue_33) : (!db.array<2,"int32[]">) -> ()

        // ELEMENT-WISE MUL

        // {1.1,2.2,3.3} ** {1.1,2.2,3.3} -> {1.21,4.84,10.89}

        //CHECK: array("{1.21, 4.84, 10.889999}")
        %oneDim_34 = db.runtime_call "ArrayEWMul" (%floatArray1, %dim1, %floatType, %floatArray1, %dim1, %floatType) : (!db.array<1,"float[]">, i64, !db.string, !db.array<1,"float[]">, i64, !db.string) -> !db.array<1,"float[]">
        db.runtime_call "DumpValue" (%oneDim_34) : (!db.array<1,"float[]">) -> ()

        // {{1,2,3},{4,5,6}} ** {{1,2,3},{4,5,6}} -> {{1, 4, 9}, {16, 25, 36}}

        //CHECK: array("{{[{]}}{1, 4, 9}, {16, 25, 36}}")
        %twoDim_35 = db.runtime_call "ArrayEWMul" (%intArray2, %dim2, %intType, %intArray2, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string, !db.array<2,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%twoDim_35) : (!db.array<2,"int32[]">) -> ()

        // {{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}} ** {{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}} 
        // -> {{{1, 4, 9, 16}, {25, 36, 49, 64}, {81, 100, 121, 144}}, {{1, 4, 9, 16}, {25, 36, 49, 64}, {81, 100, 121, 144}}}

        //CHECK: array("{{[{]}}{{[{]}}{1, 4, 9, 16}, {25, 36, 49, 64}, {81, 100, 121, 144}}, {{[{]}}{1, 4, 9, 16}, {25, 36, 49, 64}, {81, 100, 121, 144}}}")
        %threeDim_36 = db.runtime_call "ArrayEWMul" (%intArray3, %dim3, %intType, %intArray3, %dim3, %intType) : (!db.array<3,"int32[]">, i64, !db.string, !db.array<3,"int32[]">, i64, !db.string) -> !db.array<3,"int32[]">
        db.runtime_call "DumpValue" (%threeDim_36) : (!db.array<3,"int32[]">) -> ()

        // {{1,2,3},{4,5,6}} ** {{1,2,3},Null} -> {{1,4,9},null}

        //CHECK: array("{{[{]}}{1, 4, 9}, null}")
        %NullElem_37 = db.runtime_call "ArrayEWMul" (%intArray2, %dim2, %intType, %intArray2Null1, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string, !db.array<2,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%NullElem_37) : (!db.array<2,"int32[]">) -> ()

        // {{1,2,3},{4,5,6}} ** {{1,2,3},{4,Null,6}} -> {{1,4,9},{16,null,36}}

        //CHECK: array("{{[{]}}{1, 4, 9}, {16, null, 36}}")
        %NullValue_38 = db.runtime_call "ArrayEWMul" (%intArray2, %dim2, %intType, %intArray2Null2, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string, !db.array<2,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%NullValue_38) : (!db.array<2,"int32[]">) -> ()

        // MATRIX MULTIPLICATION

        %vector1 = db.constant ( "{1,2,3}" ) : !db.array<1,"int32[]">
        %vector1T = db.constant ( "{{1},{2},{3}}" ) : !db.array<2,"int32[]">

        %matrix1 = db.constant ( "{{3,2,1},{4,5,6}}" ) : !db.array<2,"int32[]">
        %matrix2 = db.constant ( "{{4,5},{6,7},{8,9},{10,11}}" ) : !db.array<2,"int32[]">

        // {1,2,3} * {{1},{2},{3}} -> {{1, 2, 3}, {2, 4, 6}, {3, 6, 9}}

        //CHECK: array("{{[{]}}{1, 2, 3}, {2, 4, 6}, {3, 6, 9}}")
        %vecToMa_39 = db.runtime_call "ArrayMatrixMul" (%vector1, %dim1, %intType, %vector1T, %dim2, %intType) : (!db.array<1,"int32[]">, i64, !db.string, !db.array<2,"int32[]">, i64, !db.string) -> !db.array<1,"int32[]">
        db.runtime_call "DumpValue" (%vecToMa_39) : (!db.array<1,"int32[]">) -> ()

        // {{1},{2},{3}} * {1,2,3} -> 14

        //CHECK: array("14")
        %vecToSc_40 = db.runtime_call "ArrayMatrixMul" (%vector1T, %dim2, %intType, %vector1, %dim1, %intType) : (!db.array<2,"int32[]">, i64, !db.string, !db.array<1,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%vecToSc_40) : (!db.array<2,"int32[]">) -> ()

        // {{3,2,1},{4,5,6}} * {{4,5},{6,7},{8,9},{10,11}} -> {{32, 33, 34}, {46, 47, 48}, {60, 61, 62}, {74, 75, 76}}

        //CHECK: array("{{[{]}}{32, 33, 34}, {46, 47, 48}, {60, 61, 62}, {74, 75, 76}}")
        %MaWMa_41 = db.runtime_call "ArrayMatrixMul" (%matrix1, %dim2, %intType, %matrix2, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string, !db.array<2,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%MaWMa_41) : (!db.array<2,"int32[]">) -> ()

        // {{1},{2},{3}} * {{3,2,1},{4,5,6}} -> {{10}, {32}}

        //CHECK: array("{{[{]}}{10}, {32}}")
        %VecWMa_42 = db.runtime_call "ArrayMatrixMul" (%vector1T, %dim2, %intType, %matrix1, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string, !db.array<2,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%VecWMa_42) : (!db.array<2,"int32[]">) -> ()

        // SCALAR MULT WITH INT

        %intScalar = db.constant (4) : i64

        // {1.1,2.2,3.3} * 4 -> {4.4,8.8,13.2}

        //CHECK: array("{4.4, 8.8, 13.2}")
        %oneDim_43 = db.runtime_call "ArrayScalarMultInt" (%floatArray1, %dim1, %floatType, %intScalar) : (!db.array<1,"float[]">, i64, !db.string, i64) -> !db.array<1,"float[]">
        db.runtime_call "DumpValue" (%oneDim_43) : (!db.array<1,"float[]">) -> ()

        // {{1,2,3},{4,5,6}} * 4 -> {{4, 8, 12}, {16, 20, 24}}

        //CHECK: array("{{[{]}}{4, 8, 12}, {16, 20, 24}}")
        %twoDim_44 = db.runtime_call "ArrayScalarMultInt" (%intArray2, %dim2, %intType, %intScalar) : (!db.array<2,"int32[]">, i64, !db.string, i64) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%twoDim_44) : (!db.array<2,"int32[]">) -> ()

        // {{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}} * 4
        // -> {{{4, 8, 12, 16}, {20, 24, 28, 32}, {36, 40, 44, 48}}, {{4, 8, 12, 16}, {20, 24, 28, 32}, {36, 40, 44, 48}}}

        //CHECK: array("{{[{]}}{{[{]}}{4, 8, 12, 16}, {20, 24, 28, 32}, {36, 40, 44, 48}}, {{[{]}}{4, 8, 12, 16}, {20, 24, 28, 32}, {36, 40, 44, 48}}}")
        %threeDim_45 = db.runtime_call "ArrayScalarMultInt" (%intArray3, %dim3, %intType, %intScalar) : (!db.array<3,"int32[]">, i64, !db.string, i64) -> !db.array<3,"int32[]">
        db.runtime_call "DumpValue" (%threeDim_45) : (!db.array<3,"int32[]">) -> ()

        // {{1,2,3},Null} * 4 -> {{4,8,12},null}

        //CHECK: array("{{[{]}}{4, 8, 12}, null}")
        %NullElem_46 = db.runtime_call "ArrayScalarMultInt" (%intArray2Null1, %dim2, %intType, %intScalar) : (!db.array<2,"int32[]">, i64, !db.string, i64) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%NullElem_46) : (!db.array<2,"int32[]">) -> ()

        // SCALAR MULT WITH FLOAT
        %floatScalar = db.constant ("4.4") : f64

        // {1.1,2.2,3.3} * 4.4 -> {4.84,9.68,14.52}

        //CHECK: array("{4.84, 9.68, 14.52}")
        %oneDim_47 = db.runtime_call "ArrayScalarMultFloat" (%floatArray1, %dim1, %floatType, %floatScalar) : (!db.array<1,"float[]">, i64, !db.string, f64) -> !db.array<1,"float[]">
        db.runtime_call "DumpValue" (%oneDim_47) : (!db.array<1,"float[]">) -> ()

        // {{1,2,3},{4,5,6}} * 4.4 -> {{4, 8, 13}, {17, 22, 26}}

        //CHECK: array("{{[{]}}{4, 8, 13}, {17, 22, 26}}")
        %twoDim_48 = db.runtime_call "ArrayScalarMultFloat" (%intArray2, %dim2, %intType, %floatScalar) : (!db.array<2,"int32[]">, i64, !db.string, f64) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%twoDim_48) : (!db.array<2,"int32[]">) -> ()

        // {{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}} * 4.4
        // -> {{{4, 8, 13, 17}, {22, 26, 30, 35}, {39, 44, 48, 52}}, {{4, 8, 13, 17}, {22, 26, 30, 35}, {39, 44, 48, 52}}}

        //CHECK: array("{{[{]}}{{[{]}}{4, 8, 13, 17}, {22, 26, 30, 35}, {39, 44, 48, 52}}, {{[{]}}{4, 8, 13, 17}, {22, 26, 30, 35}, {39, 44, 48, 52}}}")
        %threeDim_49 = db.runtime_call "ArrayScalarMultFloat" (%intArray3, %dim3, %intType, %floatScalar) : (!db.array<3,"int32[]">, i64, !db.string, f64) -> !db.array<3,"int32[]">
        db.runtime_call "DumpValue" (%threeDim_49) : (!db.array<3,"int32[]">) -> ()

        // {{1,2,3},Null} * 4 -> {{4,8,13},null}

        //CHECK: array("{{[{]}}{4, 8, 13}, null}")
        %NullElem_50 = db.runtime_call "ArrayScalarMultFloat" (%intArray2Null1, %dim2, %intType, %floatScalar) : (!db.array<2,"int32[]">, i64, !db.string, f64) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%NullElem_50) : (!db.array<2,"int32[]">) -> ()

        // TRANSPOSE

        // {1.1,2.2,3.3} -> {{1.1},{2.2},{3.3}}

        //CHECK: array("{{[{]}}{1.1}, {2.2}, {3.3}}")
        %oneDim_51 = db.runtime_call "ArrayTranspose" (%floatArray1, %dim1, %floatType) : (!db.array<1,"float[]">, i64, !db.string) -> !db.array<1,"float[]">
        db.runtime_call "DumpValue" (%oneDim_51) : (!db.array<1,"float[]">) -> ()

        // {{1,2,3},{4,5,6}} -> {{1,4}, {2,5}, {3,6}}

        //CHECK: array("{{[{]}}{1, 4}, {2, 5}, {3, 6}}")
        %twoDim_52 = db.runtime_call "ArrayTranspose" (%intArray2, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%twoDim_52) : (!db.array<2,"int32[]">) -> ()

        // {{{1,2,3,4},{5,6,7,8},{9,10,11,12}},{{1,2,3,4},{5,6,7,8},{9,10,11,12}}}
        // -> {{{1,2,3,4},{1,2,3,4}},{{5,6,7,8},{5,6,7,8}},{{9,10,11,12},{9,10,11,12}}}

        //CHECK: array("{{[{]}}{{[{]}}{1, 2, 3, 4}, {1, 2, 3, 4}}, {{[{]}}{5, 6, 7, 8}, {5, 6, 7, 8}}, {{[{]}}{9, 10, 11, 12}, {9, 10, 11, 12}}}")
        %threeDim_53 = db.runtime_call "ArrayTranspose" (%intArray3, %dim3, %intType) : (!db.array<3,"int32[]">, i64, !db.string) -> !db.array<3,"int32[]">
        db.runtime_call "DumpValue" (%threeDim_53) : (!db.array<3,"int32[]">) -> ()

        // {{1,2,3},Null} -> {{1,null},{2,null},{3,null}}

        //CHECK: array("{{[{]}}{1, null}, {2, null}, {3, null}}")
        %NullElem_54 = db.runtime_call "ArrayTranspose" (%intArray2Null1, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%NullElem_54) : (!db.array<2,"int32[]">) -> ()

        %intArray4 = db.constant ( "{{2},{3},{4}}" ) : !db.array<2,"int32[]">

        // {{2},{3},{4}} -> {2, 3, 4}

        //CHECK: array("{2, 3, 4}")
        %intTwoDimSpec_55 = db.runtime_call "ArrayTranspose" (%intArray4, %dim2, %intType) : (!db.array<2,"int32[]">, i64, !db.string) -> !db.array<2,"int32[]">
        db.runtime_call "DumpValue" (%intTwoDimSpec_55) : (!db.array<2,"int32[]">) -> ()

        // CASTS

        //CHECK: int(3)
        %int32Value = db.constant ("3") : !db.array<2,"int32[]">
        %intCast_56 = db.cast %int32Value : !db.array<2,"int32[]"> -> i32
        db.runtime_call "DumpValue" (%intCast_56) : (i32) -> ()

        //CHECK: int(281474976710656)
        %int64Value = db.constant ("281474976710656") : !db.array<3,"int32[]">
        %intCast_57 = db.cast %int64Value : !db.array<3,"int32[]"> -> i64
        db.runtime_call "DumpValue" (%intCast_57) : (i64) -> ()

        // CHECK: float(4.5)
        %floatValue = db.constant ("4.5") : !db.array<1,"int32[]">
        %floatCast_58 = db.cast %floatValue : !db.array<1,"int32[]"> -> f32
        db.runtime_call "DumpValue" (%floatCast_58) : (f32) -> ()

        //CHECK: float(44.5566)
        %doubleValue = db.constant ("44.5566") : !db.array<2,"int32[]">
        %doubleCast_59 = db.cast %doubleValue : !db.array<2,"int32[]"> -> f64
        db.runtime_call "DumpValue" (%doubleCast_59) : (f64) -> ()

        //CHECK: array("{1, 2, 3}")
        %arrayValue = db.constant ("{1,2,3}") : !db.array<2,"int32[]">
        %arrayCast_60 = db.cast %arrayValue : !db.array<2,"int32[]"> -> !db.array<1,"int32[]">
        db.runtime_call "DumpValue" (%arrayCast_60) : (!db.array<1,"int32[]">) -> ()

        //CHECK: array("{3}")
        %int32Value_2 = db.constant ("3") : i32
        %intCast_61 = db.cast %int32Value_2 : i32 -> !db.array<1,"int32[]">
        db.runtime_call "DumpValue" (%intCast_61) : (!db.array<1,"int32[]">) -> ()

        //CHECK: array("{{[{]}}{281474976710656}}")
        %int64Value_2 = db.constant ("281474976710656") : i64
        %intCast_62 = db.cast %int64Value_2 : i64 -> !db.array<2,"int64[]">
        db.runtime_call "DumpValue" (%intCast_62) : (!db.array<2,"int64[]">) -> ()

        // CHECK: array("{4.500000}")
        %floatValue_2 = db.constant ("4.5") : f32
        %floatCast_63 = db.cast %floatValue_2 : f32 -> !db.array<1,"float[]">
        db.runtime_call "DumpValue" (%floatCast_63) : (!db.array<1,"float[]">) -> ()

        //CHECK: array("{{[{]}}{44.556600}}")
        %doubleValue_2 = db.constant ("44.5566") : f64
        %doubleCast_64 = db.cast %doubleValue_2 : f64 -> !db.array<2,"double[]">
        db.runtime_call "DumpValue" (%doubleCast_64) : (!db.array<2,"double[]">) -> ()

        // FILL

        // fill(1.1, {1,2}) -> {{1,1, 1.1}}

        //CHECK: array("{{[{]}}{1.1, 1.1}}")
        %const_1 = db.constant ("1.1") : !db.string
        %structure_1 = db.constant ("{1,2}") : !db.array<1,"int64[]">
        %float = db.constant ("float") : !db.string
        %fill_65 = db.runtime_call "ArrayFill" (%structure_1, %dim1, %const_1, %float) : (!db.array<1,"int64[]">, i64, !db.string, !db.string) -> !db.array<2,"float[]">
        db.runtime_call "DumpValue" (%fill_65) : (!db.array<2,"float[]">) -> ()

        // fill(5, {4}) -> {5,5,5,5}

        //CHECK: array("{5, 5, 5, 5}")
        %const_2 = db.constant ("5") : !db.string
        %structure_2 = db.constant ("{4}") : !db.array<1,"int64[]">
        %int = db.constant ("int32") : !db.string
        %fill_66 = db.runtime_call "ArrayFill" (%structure_2, %dim1, %const_2, %int) : (!db.array<1,"int64[]">, i64, !db.string, !db.string) -> !db.array<1,"int32[]">
        db.runtime_call "DumpValue" (%fill_66) : (!db.array<1,"int32[]">) -> ()

        // fill('Hello', {3}) -> {"Hello", "Hello", "Hello"}

        //CHECK: array("{"Hello", "Hello", "Hello"}")
        %const_3 = db.constant ("Hello") : !db.string
        %structure_3 = db.constant ("{3}") : !db.array<1,"int64[]">
        %string = db.constant ("string") : !db.string
        %fill_67 = db.runtime_call "ArrayFill" (%structure_3, %dim1, %const_3, %string) : (!db.array<1,"int64[]">, i64, !db.string, !db.string) -> !db.array<1,"string[]">
        db.runtime_call "DumpValue" (%fill_67) : (!db.array<1,"string[]">) -> ()

        return
    }
}