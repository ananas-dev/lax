#include <asmjit/asmjit.h>
#include <cstdio>

#include "Analysis.h"
#include "CpuJitRuntime.h"
#include "Rom.h"

using namespace asmjit;

struct InstructionMetadata
{
    bool is_branch_target;
    bool uses_carry_flag;
    bool uses_zero_flag;
    bool uses_overflow_flag;
    bool uses_negative_flag;
};

int main(int argc, char* argv[])
{
    auto rom = Rom::from_file("nestest.nes");

    uint16_t addr = 0xC000;

    if (!rom.has_value())
    {
        printf("Failed to parse rom!\n");
        exit(1);
    }

    CpuJitRuntime cpu(addr, &rom.value());

    cpu.execute_next_bloc();

    return 0;
}
