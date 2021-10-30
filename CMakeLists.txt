cmake_minimum_required(VERSION 3.20)

include(FetchContent)
FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11
  GIT_TAG        v2.1.2
)

FetchContent_MakeAvailable(cli11)

# enable unit test
# https://stackoverflow.com/a/30264765
# enable_testing()

set(EXEC_NAME iuctl)
set(VERSION 0.0.1)

# set the project name / version
project(${EXEC_NAME} VERSION ${VERSION} LANGUAGES CXX)

add_executable(${EXEC_NAME} main.cpp)

target_include_directories(${EXEC_NAME} AFTER PRIVATE ${cli11_SOURCE_DIR}/include)
