#pragma once

#include <array>

#include "Rom.h"
#include "asmjit/core/jitruntime.h"
#include "asmjit/core/logger.h"

struct CpuState
{
    uint16_t pc;
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t s;
    uint8_t status;
};

typedef void (*JittedFunc)();

class CpuJitRuntime {
public:
    CpuJitRuntime(uint16_t pc, Rom *rom);

    void execute_next_block();

private:
    void trampoline(JittedFunc f);

    asmjit::JitRuntime m_jit_runtime{};
    asmjit::FileLogger m_logger{stdout};
    CpuState m_cpu_state{};
    std::array<uint8_t, 0x800> m_ram{};
    Rom *m_rom;
};
