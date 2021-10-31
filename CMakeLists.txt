cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

set(COMPILE_OPTIONS -g -Wall -Wconversion -Wdouble-promotion -Weffc++ -Wextra -Wfloat-equal -Wformat=2 -Wno-error=unused-variable -Wnull-dereference -Wold-style-cast -Wpedantic -Wshadow -Wuninitialized -Wunreachable-code -Wredundant-move -Wpessimizing-move)


include(FetchContent)
FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11
  GIT_TAG        70f8072f9dd2292fd0b9f9e5f58e279f60483ed3
)

FetchContent_Declare(
  rapidjson
  GIT_REPOSITORY https://github.com/Tencent/rapidjson/
  GIT_TAG        f54b0e47a08782a6131cc3d60f94d038fa6e0a51
)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        d141cdbeb0fb422a3fb7173b285fd38e0d1772dc
)

FetchContent_GetProperties(cli11)
if(NOT cli11_POPULATED)
    FetchContent_Populate(cli11)
endif()

FetchContent_GetProperties(rapidjson)
if(NOT rapidjson_POPULATED)
    FetchContent_Populate(rapidjson)
endif()

FetchContent_MakeAvailable(fmt)

# enable unit test
# https://stackoverflow.com/a/30264765
# enable_testing()

set(EXEC_NAME iuctl)
set(LIB_NAME iulib)
set(VERSION 0.0.1)

# set the project name / version
project(${EXEC_NAME} VERSION ${VERSION} LANGUAGES CXX)

add_subdirectory(src)
add_executable(${EXEC_NAME} main.cpp)

target_include_directories(${EXEC_NAME} AFTER PRIVATE ${cli11_SOURCE_DIR}/include)
target_include_directories(${EXEC_NAME} AFTER PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(${EXEC_NAME} PRIVATE ${COMPILE_OPTIONS})
target_link_libraries(${EXEC_NAME} PRIVATE ${LIB_NAME})

# install
install(TARGETS ${EXEC_NAME} RUNTIME DESTINATION "~/bin")
