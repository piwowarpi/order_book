set(HEADER_LIST
        "${order_book_SOURCE_DIR}/include/MsgReader.h"
        "${order_book_SOURCE_DIR}/include/Order.h"
        "${order_book_SOURCE_DIR}/include/OrderBook.h")

add_library(quant_library
        MsgReader.cpp
        Order.cpp
        OrderBook.cpp
        ${HEADER_LIST})

target_include_directories(quant_library PUBLIC ../include)
target_compile_features(quant_library PUBLIC cxx_std_17)

# Input file
target_compile_definitions(quant_library PUBLIC INPUT_FILE="${PROJECT_SOURCE_DIR}/input_files/ticks.raw")

# Output file
target_compile_definitions(quant_library PUBLIC OUTPUT_FILE="${PROJECT_SOURCE_DIR}/result_files/ticks.csv")

source_group(TREE "${PROJECT_SOURCE_DIR}/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST})

