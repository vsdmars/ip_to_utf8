cmake_minimum_required(VERSION 3.20)

add_library(${LIB_NAME} STATIC iputf8.cpp)

target_compile_options(${LIB_NAME} PRIVATE ${COMPILE_OPTIONS})
target_include_directories(${LIB_NAME} AFTER PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(${LIB_NAME} AFTER PRIVATE ${rapidjson_SOURCE_DIR}/include)
target_include_directories(${LIB_NAME} AFTER PRIVATE ${fmt_SOURCE_DIR}/include)
target_link_libraries(${LIB_NAME} PRIVATE $<BUILD_INTERFACE:fmt::fmt-header-only>)
