#pragma once
#include <asmjit/x86/x86assembler.h>
#include <vector>
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
    static constexpr auto MemoryBase = asmjit::x86::r12;
} // namespace Cpu

class Generator
{
public:
    explicit Generator(Analysis analysis, asmjit::x86::Assembler& a, std::array<uint8_t, 0x800>& ram);
    void emit_next();

    uint16_t pc;
    uint16_t entry_point;
    uint16_t exit_point;
    std::unordered_map<uint16_t, Bitset8<InstructionMetadataFields>> instructions;
    std::array<uint8_t, 0x800>& ram;
    Rom* rom;
    asmjit::x86::Assembler& a;

    bool should_stop{false};

    std::vector<asmjit::Label> labels;

    uint8_t read();
    uint16_t read_u16();

    void emit_update_nz(const asmjit::x86::Gpd& reg);

    void emit_return();

    void emit_stack_push(uint8_t value);
    void emit_stack_push_address(uint16_t address);

    static uint64_t memory_offset(uint16_t address);

    void generate()
    {
        printf("Range: %u - %u\n", entry_point, exit_point);

        while (pc < exit_point && !should_stop)
        {
            printf("PC: %u\n", pc);
            emit_next();
        }
    }
};
