#pragma once

#include <cinttypes>

struct CpuState
{
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t s;
    uint8_t status;
};

typedef void (*JittedFunc)();

class CpuJitRuntime {
public:

private:
    void trampoline();
    uint16_t m_pc;
};
