#include "CpuJitRuntime.h"

#include "Analysis.h"
#include "Generator.h"
#include "asmjit/core/logger.h"
#include "asmjit/x86/x86assembler.h"

CpuJitRuntime::CpuJitRuntime(uint16_t pc, Rom* rom) : m_rom(rom)
{
    m_cpu_state.a = 0;
    m_cpu_state.x = 0;
    m_cpu_state.y = 0;
    m_cpu_state.s = 0xFD;
    m_cpu_state.status = 36;
    m_cpu_state.pc = pc;
}

void CpuJitRuntime::execute_next_block()
{
    asmjit::CodeHolder code;

    code.init(m_jit_runtime.environment(), m_jit_runtime.cpuFeatures());
    code.setLogger(&m_logger);

    Analysis analysis(m_cpu_state.pc, m_rom);
    analysis.perform();

    asmjit::x86::Assembler a(&code);

    Generator gen(std::move(analysis), a, m_ram);
    gen.generate();

    JittedFunc f;
    asmjit::Error err = m_jit_runtime.add(&f, &code);

    if (err)
    {
        printf("AsmJit failed: %s\n", asmjit::DebugUtils::errorAsString(err));
        return;
    };

    trampoline(f);

    printf("[%04X] A=%02X X=%02X Y=%02X S=%02X STATUS=%02X\n", m_cpu_state.pc, m_cpu_state.a, m_cpu_state.x,
           m_cpu_state.y, m_cpu_state.s, m_cpu_state.status);
}


void CpuJitRuntime::trampoline(JittedFunc f)
{
    asm volatile(
        "movzwl %c[pc_off](%[cpu]), %%r8d\n\t"
        "movzbl %c[a_off](%[cpu]), %%eax\n\t"
        "movzbl %c[x_off](%[cpu]), %%ecx\n\t"
        "movzbl %c[y_off](%[cpu]), %%edx\n\t"
        "movzbl %c[s_off](%[cpu]), %%esi\n\t"
        "movzbl %c[status_off](%[cpu]), %%edi\n\t"
        "mov %[memory_base], %%r12\n\t"

        "call *%[func]\n\t"

        "movw %%r8w, %c[pc_off](%[cpu])\n\t"
        "movb %%al, %c[a_off](%[cpu])\n\t"
        "movb %%cl, %c[x_off](%[cpu])\n\t"
        "movb %%dl, %c[y_off](%[cpu])\n\t"
        "movb %%sil, %c[s_off](%[cpu])\n\t"
        "movb %%dil, %c[status_off](%[cpu])\n\t"
        :
        : [cpu] "r"(&m_cpu_state), [func] "r"(f), [memory_base] "r"(m_ram.data()), [a_off] "i"(offsetof(CpuState, a)),
          [x_off] "i"(offsetof(CpuState, x)), [y_off] "i"(offsetof(CpuState, y)), [s_off] "i"(offsetof(CpuState, s)),
          [status_off] "i"(offsetof(CpuState, status)), [pc_off] "i"(offsetof(CpuState, pc))
        : "eax", "ecx", "edx", "esi", "edi", "r8d", "r9d", "r10d", "r11d", "r12", "memory");
}