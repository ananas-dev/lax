#pragma once
#include <asmjit/x86/x86assembler.h>
#include "Analysis.h"

namespace Cpu
{
    static constexpr auto A = asmjit::x86::eax;
    static constexpr auto X = asmjit::x86::ecx;
    static constexpr auto Y = asmjit::x86::edx;
    static constexpr auto S = asmjit::x86::esi;
    static constexpr auto Status = asmjit::x86::edi;
    static constexpr auto PC = asmjit::x86::r8d;
    static constexpr auto Temp1 = asmjit::x86::r9d;
    static constexpr auto Temp2 = asmjit::x86::r10d;
    static constexpr auto Temp3 = asmjit::x86::r11d;
} // namespace Cpu

class Generator
{
public:
    explicit Generator(Analysis analysis);
    void emit_next();

    uint32_t pc;
    std::unordered_map<uint16_t, Bitset8<InstructionMetadataFields>> instructions;
    std::span<uint8_t> memory;
    asmjit::x86::Assembler a;

    uint8_t read();
    uint16_t read_u16();

    void emit_update_nz(const asmjit::x86::Gpd& reg);

private:
    using InstructionEmitFn = void (Generator::*)();
};
