cmake_minimum_required(VERSION 3.10)

project(CloneDetector)

set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(include)

add_definitions(${LLVM_DEFINITIONS})

add_executable(clone_detector
    src/main.cpp
    src/ir_parser/IRParser.cpp
    src/fingerprint/Fingerprint.cpp
    src/similarity/SimilarityEngine.cpp
    src/normalizer/Normalizer.cpp
    src/cfg_builder/CFGBuilder.cpp
)

llvm_map_components_to_libnames(llvm_libs
    core
    support
    irreader
)

target_link_libraries(clone_detector ${llvm_libs})