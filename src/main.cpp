#include <asmjit/asmjit.h>
#include <cstdio>

#include "Analysis.h"
#include "CpuJitRuntime.h"
#include "Rom.h"

using namespace asmjit;

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

    cpu.execute_next_block();
    cpu.execute_next_block();
    cpu.execute_next_block();

    return 0;
}
