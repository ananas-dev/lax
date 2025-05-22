cmake_minimum_required(VERSION 3.30)

project(lax C CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-fno-exceptions")

include(FetchContent)

FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.14
)

FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
        asmjit
        GIT_REPOSITORY https://github.com/asmjit/asmjit
        GIT_TAG cecc73f2979e9704c81a2c2ec79a7475b31c56ac
)

FetchContent_MakeAvailable(asmjit)

set(ASMJIT_STATIC TRUE)

add_executable(lax
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

target_link_libraries(lax
        asmjit::asmjit
        SDL3::SDL3
)
