#include "Generator.h"

#include <array>
#include <asmjit/asmjit.h>
#include <cassert>

#include "Instruction.h"

typedef void (*GeneratorOpFunc)(Generator&);

Generator::Generator(Analysis analysis) :
    pc(analysis.entry_point), instructions(std::move(analysis.instructions)), memory(analysis.memory)
{
}

void Generator::emit_update_nz(const asmjit::x86::Gpd& reg)
{
    a.mov(Cpu::Temp1, reg);

    // Temp3 = (reg == 0 ? 1 : 0)
    a.xor_(Cpu::Temp3, Cpu::Temp3);
    a.test(reg, reg);
    a.sete(Cpu::Temp3.r8());

    // Isolate the 8th bit
    a.and_(Cpu::Temp1, 1 << 7);

    // Neat trick to merge N and Z together
    a.lea(Cpu::Temp2, {Cpu::Temp1.r64(), Cpu::Temp3.r64(), 1, 0});

    // Clear NZ in the status
    a.and_(Cpu::Status, ~(1 << 7 | 1 << 1));

    // Merge back the new NZ in the status
    a.or_(Cpu::Status, Cpu::Temp2);
}


/*
enum class AddressingMode
{
    Impl = 0,
    Imm,
    Zpg,
    ZpgX,
    ZpgY,
    Abs,
    AbsX,
    AbsY,
    Ind,
    IndX,
    IndY,
    Rel,
    Acc,
};
*/

uint8_t Generator::read() { return memory[pc++]; }

uint16_t Generator::read_u16()
{
    uint16_t low = memory[pc++];
    uint16_t high = memory[pc++];

    return (high << 8) | low;
}

static void* addressing_Impl(Generator& generator) { return nullptr; }

static asmjit::Imm addressing_Imm(Generator& generator) { return {generator.read()}; }

static asmjit::x86::Mem addressing_Zpg(Generator& generator)
{
    uint64_t offset = generator.read();
    return asmjit::x86::Mem(reinterpret_cast<uint64_t>(generator.memory.data()) + offset);
};

template <Op op, AddressingMode addr>
static void emit(Generator& generator)
{

}

template <AddressingMode addr>
auto void get_operand(Generator& generator)
{
    printf("0x%02x is not implemented!\n", op);
    exit(1);
}


template <Op op>
static void emit_op(Generator& generator, const auto& operand)
{
    printf("0x%02x is not implemented!\n", op);
    exit(1);
}

static void emit_op<Op::Lda>(Generator &generator, const auto& operand)
{
    generator.a.mov(Cpu::A, operand);
    generator.emit_update_nz(Cpu::A);
}

// template <typename Operand>
// static void emit_Ldx(Generator& generator, const Operand& operand)
// {
//     generator.a.mov(Cpu::X, operand);
//     generator.emit_update_nz(Cpu::X);
// }
//
// template <typename Operand>
// static void emit_Ldy(Generator& generator, const Operand& operand)
// {
//     generator.a.mov(Cpu::Y, operand);
//     generator.emit_update_nz(Cpu::Y);
// }
//
// template <typename Operand>
// static void emit_Sta(Generator& generator, const Operand& operand)
// {
//     generator.a.mov(Cpu::A, operand);
// }
//
// template <typename Operand>
// static void emit_Stx(Generator& generator, const Operand& operand)
// {
//     generator.a.mov(Cpu::A, operand);
// }
//
// template <typename Operand>
// static void emit_Sty(Generator& generator, const Operand& operand)
// {
//     generator.a.mov(Cpu::A, operand);
// }
//
// template <typename Operand>
// static void emit_Tax(Generator& generator, const Operand& operand)
// {
//     generator.a.mov(Cpu::X, Cpu::A);
// }
//
// template <typename Operand>
// static void emit_Tay(Generator& generator, const Operand& operand)
// {
//     generator.a.mov(Cpu::Y, Cpu::A);
// }
//
// template <typename Operand>
// static void emit_Txa(Generator& generator, const Operand& operand)
// {
//     generator.a.mov(Cpu::A, Cpu::X);
// }
//
// template <typename Operand>
// static void emit_Tya(Generator& generator, const Operand& operand)
// {
//     generator.a.mov(Cpu::A, Cpu::Y);
// }
//
// template <typename Operand>
// static void emit_Tsx(Generator& generator, const Operand& operand)
// {
//     generator.a.mov(Cpu::X, Cpu::S);
// }
//
// template <typename Operand>
// static void emit_Txs(Generator& generator, const Operand& operand)
// {
//     generator.a.mov(Cpu::S, Cpu::X);
// }

#define X(index, op, addressing_mode, size)                                                                            \
    static void emit_##op##_##addressing_mode(Generator& generator)                                                    \
    {                                                                                                                  \
        auto operand = addressing_##addressing_mode(generator);                                                        \
        emit_##op(generator, operand);                                                                                 \
    }
OPCODES
#undef X

static constexpr std::array<GeneratorOpFunc, NUM_OPCODES> emit_function_table = {
#define X(index, op, addressing_mode, size) emit_##op_##addressing_mode,
    OPCODES
#undef X
};

void Generator::emit_next()
{
    uint8_t instr = 0;
    emit_function_table[instr](*this);
}
