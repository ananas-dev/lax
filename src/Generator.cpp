#include "Generator.h"

#include <array>
#include <asmjit/asmjit.h>
#include <cassert>
#include <optional>

#include "Instruction.h"

typedef void (*EmitFn)(Generator&);

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

    [[nodiscard]] asmjit::x86::Mem to_jit_mem(Generator& gen) const { return asmjit::x86::Mem(gen.ram_ptr(value)); }

    [[nodiscard]] std::optional<asmjit::Label> to_jit_label(Generator& gen) const
    {
        if (value >= gen.entry_point && value < gen.exit_point)
        {
            return gen.labels[gen.entry_point - value];
        }

        return {};
    }
};

struct Impl
{
};

Generator::Generator(Analysis analysis, asmjit::x86::Assembler& a, std::array<uint8_t, 0x800>& ram) :
    pc(analysis.entry_point), entry_point(analysis.entry_point), exit_point(analysis.pc),
    instructions(std::move(analysis.instructions)), ram(ram), rom(analysis.rom), a(a)
{
    std::vector<asmjit::Label> labels(exit_point - entry_point);

    for (uint16_t i = entry_point; i < exit_point; i++)
    {
        labels.push_back(a.newLabel());
    }
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

void Generator::emit_return()
{
    a.ret();
    should_stop = true;
}

uint8_t Generator::read()
{
    uint8_t result = rom->read_prg(pc);
    pc += 1;

    return result;
}

uint16_t Generator::read_u16()
{
    uint8_t result = rom->read_prg_u16(pc);
    pc += 2;

    return result;
}

uint64_t Generator::ram_ptr(uint16_t address)
{
    if (address > 0x3FFF)
    {
        printf("Ram read out of bounds!\n");
        return 0;
    }

    uint16_t mirror_down_addr = address & 0b0000011111111111;
    return reinterpret_cast<uint64_t>(ram.data()) + mirror_down_addr;
}

template <AddressingMode addr>
static auto get_operand(Generator& gen)
{
    printf("Addressing mode %d is not implemented!\n", static_cast<int>(addr));
    exit(1);

    // Dummy value to stand out
    return Address{69};
}

template <>
auto get_operand<AddressingMode::Impl>(Generator& gen)
{
    return Impl{};
}

template <>
auto get_operand<AddressingMode::Imm>(Generator& gen)
{
    return Imm{gen.read()};
}

template <>
auto get_operand<AddressingMode::Zpg>(Generator& gen)
{
    return Address{gen.read()};
}

template <>
auto get_operand<AddressingMode::Abs>(Generator& gen)
{
    return Address{gen.read_u16()};
}

template <Op op, typename Operand>
static void emit_op(Generator& gen, Operand const& operand);

template <Op op, typename Operand>
void emit_op(Generator& gen, Operand const& operand)
{
    printf("Operand %u is not implemented!\n", static_cast<int>(op));
    exit(1);
}

template <>
void emit_op<Op::Lda>(Generator& gen, Imm const& operand)
{
    gen.a.mov(Cpu::A, operand.to_jit_imm());
}

template <>
void emit_op<Op::Lda>(Generator& gen, Address const& operand)
{
    gen.a.mov(Cpu::A, operand.to_jit_mem(gen));
}

template <>
void emit_op<Op::Ldx>(Generator& gen, Imm const& operand)
{
    gen.a.mov(Cpu::X, operand.to_jit_imm());
}

template <>
void emit_op<Op::Ldx>(Generator& gen, Address const& operand)
{
    gen.a.mov(Cpu::X, operand.to_jit_mem(gen));
}

template <>
void emit_op<Op::Ldy>(Generator& gen, Imm const& operand)
{
    gen.a.mov(Cpu::Y, operand.to_jit_imm());
}

template <>
void emit_op<Op::Ldy>(Generator& gen, Address const& operand)
{
    gen.a.mov(Cpu::Y, operand.to_jit_mem(gen));
}

template <>
void emit_op<Op::Jmp>(Generator& gen, Address const& operand)
{
    auto jump_target = operand.to_jit_label(gen);

    if (jump_target.has_value())
    {
        gen.a.jmp(jump_target.value());
    }
    else
    {
        gen.a.mov(Cpu::PC, operand.value);
        gen.emit_return();
    }
}

template <Op op, AddressingMode addr>
static void emit_instruction(Generator& gen)
{
    auto operand = get_operand<addr>(gen);
    emit_op<op>(gen, operand);
}

static constexpr std::array<EmitFn, NUM_OPCODES> emit_function_table = {
#define X(index, op, addr, size) emit_instruction<Op::op, AddressingMode::addr>,
    OPCODES
#undef X
};

void Generator::emit_next()
{
    uint8_t opcode = read();

    printf("Emit %s\n", Instruction::to_string(opcode));
    emit_function_table[opcode](*this);
}
