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

enum class StatusFlag : uint8_t
{
    Carry = 1 << 0,
    Zero = 1 << 1,
    InterruptDisable = 1 << 2,
    Decimal = 1 << 3,
    Overflow = 1 << 6,
    Negative = 1 << 7
};

inline StatusFlag operator|(StatusFlag lhs, StatusFlag rhs)
{
    using T = std::underlying_type_t<StatusFlag>;
    return static_cast<StatusFlag>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline StatusFlag operator&(StatusFlag lhs, StatusFlag rhs)
{
    using T = std::underlying_type_t<StatusFlag>;
    return static_cast<StatusFlag>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

inline StatusFlag operator~(StatusFlag flag)
{
    using T = std::underlying_type_t<StatusFlag>;
    return static_cast<StatusFlag>(~static_cast<T>(flag));
}

struct Generator;

struct Imm
{
    uint8_t value;
    explicit Imm(uint8_t value) : value(value) {};

    [[nodiscard]] asmjit::Imm to_jit_imm() const { return asmjit::Imm{value}; }
};

struct Address
{
    uint16_t value;
    explicit Address(uint16_t value) : value(value) {};

    [[nodiscard]] asmjit::x86::Mem to_jit_mem() const
    {
        if (value > 0x3FFF)
        {
            printf("Ram read out of bounds!\n");
            return asmjit::x86::byte_ptr(69);
        }

        uint16_t mirror_down_addr = value & 0b0000011111111111;
        return asmjit::x86::byte_ptr(Cpu::MemoryBase, mirror_down_addr);
    }

    [[nodiscard]] std::optional<asmjit::Label> to_jit_label(Generator& gen) const;
};


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

    std::vector<asmjit::Label> labels{};

    uint8_t read();
    uint16_t read_u16();

    void emit_update_nz(const asmjit::x86::Gp& reg);

    void emit_stack_push(uint8_t value);
    void emit_stack_push_address(uint16_t address);

    void emit_stack_pop(const asmjit::x86::Gp& dst);
    void emit_stack_pop_address(const asmjit::x86::Gp& dst);

    void emit_branch_if_set(StatusFlag flag, Address const& target);
    void emit_branch_if_clear(StatusFlag flag, Address const& target);

    void generate()
    {
        printf("Range: %u - %u\n", entry_point, exit_point);

        while (pc < exit_point && !should_stop)
        {
            printf("PC: %04X\n", pc);
            emit_next();
        }

        // Debug fallback
        a.mov(Cpu::PC, pc);
        a.ret();
    }
};
