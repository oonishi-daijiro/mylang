cmake_minimum_required(VERSION 3.16)

project(learn-llvm)

set(CMAKE_CXX_OMPILER clang++)
set(CMAKE_C_COMPILER clang)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM REQUIRED)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# add_compile_options(-Wall)

file(GLOB src ./*.cpp)


llvm_map_components_to_libnames(llvm_libs support core irreader x86asmparser x86codegen orcjit core orcjit native)


add_executable(mylangc ${src})
target_link_libraries(mylangc PUBLIC ${llvm_libs})
target_include_directories(mylangc PUBLIC ${LLVM_INCLUDE_DIRS})

