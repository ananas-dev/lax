cmake_minimum_required(VERSION 3.30)

project(jit C CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-fno-exceptions")

set(ASMJIT_STATIC TRUE)

add_subdirectory("vendor/asmjit")
add_subdirectory("vendor/SDL" EXCLUDE_FROM_ALL)


add_executable(jit
        src/main.cpp
        src/Instruction.cpp
        src/Instruction.h
        src/Rom.cpp
        src/Analysis.cpp
        src/Analysis.h
        src/Bitset8.h
        src/Generator.cpp
        src/Generator.h
        src/CpuJitRuntime.cpp
        src/CpuJitRuntime.h
)

target_link_libraries(jit
        asmjit::asmjit
        SDL3::SDL3
)
