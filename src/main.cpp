#include <asmjit/asmjit.h>
#include <cstdio>

#include "Analysis.h"
#include "ByteStream.h"
#include "Generator.h"
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

void jit_trampoline(CpuState* cpu_state, Func f)
{
    asm volatile(
        "movzbl %c[a_off](%[cpu]), %%eax\n\t"
        "movzbl %c[x_off](%[cpu]), %%ecx\n\t"
        "movzbl %c[y_off](%[cpu]), %%edx\n\t"
        "movzbl %c[status_off](%[cpu]), %%esi\n\t"

        "call *%[func]\n\t"

        "movb %%al, %c[a_off](%[cpu])\n\t"
        "movb %%cl, %c[x_off](%[cpu])\n\t"
        "movb %%dl, %c[y_off](%[cpu])\n\t"
        "movb %%sil, %c[status_off](%[cpu])\n\t"
        :
        : [cpu] "r"(cpu_state), [func] "r"(f), [a_off] "i"(offsetof(CpuState, a)), [x_off] "i"(offsetof(CpuState, x)),
          [y_off] "i"(offsetof(CpuState, y)), [status_off] "i"(offsetof(CpuState, status))
        : "eax", "ecx", "edx", "esi", "edi", "r8d", "r9d", "r10d", "r11d", "memory");
}


void compile_function(CpuState* cpu_state)
{
    jit_trampoline()
}

int main(int argc, char* argv[])
{
    // Runtime designed for JIT - it holds relocated functions and controls their lifetime.
    JitRuntime rt;

    // Holds code and relocation information during code generation.
    CodeHolder code;

    // Code holder must be initialized before it can be used. The simples way to initialize
    // it is to use 'Environment' from JIT runtime, which matches the target architecture,
    // operating system, ABI, and other important properties.
    code.init(rt.environment(), rt.cpuFeatures());

    x86::Assembler a(&code);

    FileLogger logger(stdout);
    code.setLogger(&logger);

    auto rom = Rom::from_file("nestest.nes");

    uint16_t addr = 0xC000;

    if (!rom.has_value())
    {
        printf("Failed to parse rom!\n");
        exit(1);
    }

    addr -= 0x8000;
    if (addr >= 0x4000)
    {
        addr = addr % 0x4000;
    }

    Analysis analysis(addr, std::move(rom.value()));
    analysis.perform();

    Generator gen(std::move(analysis), a);
    gen.generate();

    code.detach(&a);

    std::array<uint8_t, 0x800> cpu_ram{};

    Func fn;
    Error err = rt.add(&fn, &code);

    if (err)
    {
        printf("AsmJit failed: %s\n", DebugUtils::errorAsString(err));
        return 1;
    }

    CpuState state{0, 0, 0};

    jit_trampoline(&state, fn);

    printf("State: A=%hhu, X=%hhu, Y=%hhu", state.a, state.x, state.y);

    rt.release(fn);

    return 0;
}
