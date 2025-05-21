#include "CpuJitRuntime.h"

#include "Analysis.h"
#include "Generator.h"
#include "asmjit/core/logger.h"
#include "asmjit/x86/x86assembler.h"

CpuJitRuntime::CpuJitRuntime(uint16_t pc, Rom* rom) : m_rom(rom)
{
    m_code_holder.init(m_jit_runtime.environment(), m_jit_runtime.cpuFeatures());
    m_code_holder.setLogger(&m_logger);

    m_cpu_state.a = 0;
    m_cpu_state.x = 0;
    m_cpu_state.y = 0;
    m_cpu_state.s = 0xFD;
    m_cpu_state.status = 36;
    m_cpu_state.pc = pc;
}

void CpuJitRuntime::execute_next_bloc()
{
    Analysis analysis(m_cpu_state.pc, m_rom);
    analysis.perform();

    asmjit::x86::Assembler a(&m_code_holder);

    Generator gen(std::move(analysis), a, m_ram);
    gen.generate();

    m_code_holder.detach(&a);

    JittedFunc f;
    m_jit_runtime.add(&f, &m_code_holder);

    m_code_holder.reset();

    trampoline(f);

    printf("State: A=%hhu, X=%hhu, Y=%hhu", m_cpu_state.a, m_cpu_state.x, m_cpu_state.y);
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

        "call *%[func]\n\t"

        "movw %%r8w, %c[pc_off](%[cpu])\n\t"
        "movb %%al, %c[a_off](%[cpu])\n\t"
        "movb %%cl, %c[x_off](%[cpu])\n\t"
        "movb %%dl, %c[y_off](%[cpu])\n\t"
        "movb %%dil, %c[s_off](%[cpu])\n\t"
        "movb %%sil, %c[status_off](%[cpu])\n\t"
        :
        : [cpu] "r"(&m_cpu_state), [func] "r"(f), [a_off] "i"(offsetof(CpuState, a)),
          [x_off] "i"(offsetof(CpuState, x)), [y_off] "i"(offsetof(CpuState, y)), [s_off] "i"(offsetof(CpuState, s)),
          [status_off] "i"(offsetof(CpuState, status)), [pc_off] "i"(offsetof(CpuState, pc))
        : "eax", "ecx", "edx", "esi", "edi", "r8d", "r9d", "r10d", "r11d", "memory");
}
