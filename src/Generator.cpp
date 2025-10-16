#include "Generator.h"

#include <array>
#include <asmjit/asmjit.h>
#include <cassert>
#include <optional>

#include "Instruction.h"

typedef void (*EmitFn)(Generator&);

constexpr auto STACK_ADDRESS = 0x0100;

std::optional<asmjit::Label> Address::to_jit_label(Generator& gen) const
{
    if (value >= gen.entry_point && value < gen.exit_point)
    {
        return gen.labels[value - gen.entry_point];
    }

    return {};
}

Generator::Generator(Analysis analysis, asmjit::x86::Assembler& a, std::array<uint8_t, 0x800>& ram) :
    pc(analysis.entry_point), entry_point(analysis.entry_point), exit_point(analysis.pc),
    instructions(std::move(analysis.instructions)), ram(ram), rom(analysis.rom), a(a)
{
    labels.reserve(exit_point - entry_point);

    for (uint16_t i = entry_point; i < exit_point; i++)
    {
        labels.push_back(a.new_label());
    }
}

void Generator::emit_update_nz(const asmjit::x86::Gp& reg)
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
    a.and_(Cpu::Status, ~(StatusFlag::Negative | StatusFlag::Zero));

    // Merge back the new NZ in the status
    a.or_(Cpu::Status, Cpu::Temp2);
}

uint8_t Generator::read()
{
    uint8_t result = rom->read_prg(pc);
    pc += 1;

    return result;
}

uint16_t Generator::read_u16()
{
    uint16_t result = rom->read_prg_u16(pc);
    pc += 2;

    return result;
}

void Generator::emit_stack_push(uint8_t value)
{
    a.mov(byte_ptr(Cpu::MemoryBase, Cpu::S.r64(), 0, STACK_ADDRESS), value);
    a.dec(Cpu::S.r8());
}

void Generator::emit_stack_push_address(uint16_t address)
{
    auto high = static_cast<uint8_t>(address >> 8);
    auto low = static_cast<uint8_t>(address & 0xFF);

    emit_stack_push(high);
    emit_stack_push(low);
}

void Generator::emit_stack_pop(const asmjit::x86::Gp& dst)
{
    a.inc(Cpu::S.r8());
    a.mov(dst.r8(), byte_ptr(Cpu::MemoryBase, Cpu::S.r64(), 0, STACK_ADDRESS));
}

void Generator::emit_stack_pop_address(const asmjit::x86::Gp& dst)
{
    a.inc(Cpu::S.r8());
    a.movzx(Cpu::Temp1, byte_ptr(Cpu::MemoryBase, Cpu::S.r64(), 0, STACK_ADDRESS));

    a.inc(Cpu::S.r8());
    a.movzx(dst, byte_ptr(Cpu::MemoryBase, Cpu::S.r64(), 0, STACK_ADDRESS));

    a.shl(dst, 8);
    a.or_(dst, Cpu::Temp1);
}

void Generator::emit_branch_if_set(StatusFlag flag, Address const& target)
{
    a.test(Cpu::Status, flag);

    auto branch_target = target.to_jit_label(*this);

    if (branch_target.has_value())
    {
        a.jne(branch_target.value());
    }
    else
    {
        a.mov(Cpu::PC, target.value);

        // BEGIN DEBUG
        a.mov(Cpu::Temp1, pc);
        a.cmove(Cpu::PC, Cpu::Temp1);
        // END DEBUG

        a.ret();
    }
}

void Generator::emit_branch_if_clear(StatusFlag flag, Address const& target)
{
    a.test(Cpu::Status, flag);

    auto branch_target = target.to_jit_label(*this);

    if (branch_target.has_value())
    {
        a.je(branch_target.value());
    }
    else
    {
        a.mov(Cpu::PC, target.value);

        // BEGIN DEBUG
        a.mov(Cpu::Temp1, pc);
        a.cmovne(Cpu::PC, Cpu::Temp1);
        // END DEBUG

        a.ret();
    }
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

template <>
auto get_operand<AddressingMode::Rel>(Generator& gen)
{
    auto offset = static_cast<int8_t>(gen.read());
    auto target = static_cast<uint16_t>(gen.pc + static_cast<int16_t>(offset));

    return Address{target};
}

template <Op op, typename Operand>
static void emit_op(Generator& gen, Operand const& operand);

template <Op op>
static void emit_op(Generator& gen);

template <Op op, typename Operand>
void emit_op(Generator& gen, Operand const& operand)
{
    printf("Operand %u is not implemented!\n", static_cast<int>(op));
    exit(1);
}

template <Op op>
static void emit_op(Generator& gen)
{
    printf("Operand %u is not implemented!\n", static_cast<int>(op));
    exit(1);
}

template <>
void emit_op<Op::Lda>(Generator& gen, Imm const& operand)
{
    gen.a.mov(Cpu::A, operand.to_jit_imm());
    gen.emit_update_nz(Cpu::A);
}

template <>
void emit_op<Op::Lda>(Generator& gen, Address const& operand)
{
    gen.a.mov(Cpu::A.r8(), operand.to_jit_mem());
    gen.emit_update_nz(Cpu::A);
}

template <>
void emit_op<Op::Ldx>(Generator& gen, Imm const& operand)
{
    gen.a.mov(Cpu::X, operand.to_jit_imm());
    gen.emit_update_nz(Cpu::X);
}

template <>
void emit_op<Op::Ldx>(Generator& gen, Address const& operand)
{
    gen.a.mov(Cpu::X.r8(), operand.to_jit_mem());
    gen.emit_update_nz(Cpu::X);
}

template <>
void emit_op<Op::Ldy>(Generator& gen, Imm const& operand)
{
    gen.a.mov(Cpu::Y, operand.to_jit_imm());
    gen.emit_update_nz(Cpu::Y);
}

template <>
void emit_op<Op::Ldy>(Generator& gen, Address const& operand)
{
    gen.a.mov(Cpu::Y.r8(), operand.to_jit_mem());
    gen.emit_update_nz(Cpu::Y);
}

template <>
void emit_op<Op::Sta>(Generator& gen, Address const& operand)
{
    gen.a.mov(operand.to_jit_mem(), Cpu::A.r8());
}

template <>
void emit_op<Op::Stx>(Generator& gen, Address const& operand)
{
    gen.a.mov(operand.to_jit_mem(), Cpu::X.r8());
}

template <>
void emit_op<Op::Sty>(Generator& gen, Address const& operand)
{
    gen.a.mov(operand.to_jit_mem(), Cpu::Y.r8());
}

template <>
void emit_op<Op::Bit>(Generator& gen, Address const& operand)
{
    gen.a.mov(Cpu::Temp1.r8(), operand.to_jit_mem());

    gen.a.test(Cpu::A, Cpu::Temp1);

    // Temp3 = (A == 0 ? 1 : 0)
    gen.a.xor_(Cpu::Temp3, Cpu::Temp3);
    gen.a.sete(Cpu::Temp3.r8());

    // Isolate the 8 and 7th bit to get NC
    gen.a.and_(Cpu::Temp1, ~(1 << 7 | 1 << 6));

    // Merge NC and Z together
    gen.a.lea(Cpu::Temp2, {Cpu::Temp1.r64(), Cpu::Temp3.r64(), 1, 0});

    // Clear NZC in the status
    gen.a.and_(Cpu::Status, ~(StatusFlag::Negative | StatusFlag::Zero | StatusFlag::Carry));

    // Merge back the new NZC in the status
    gen.a.or_(Cpu::Status, Cpu::Temp2);
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
        gen.a.ret();
    }
}

template <>
void emit_op<Op::Jsr>(Generator& gen, Address const& operand)
{
    gen.emit_stack_push_address(gen.pc - 1);
    gen.a.mov(Cpu::PC, operand.value);

    auto jump_target = operand.to_jit_label(gen);

    if (jump_target.has_value())
    {
        gen.a.jmp(jump_target.value());
    }
    else
    {
        gen.a.mov(Cpu::PC, operand.value);
        gen.a.ret();
    }
}

template <>
void emit_op<Op::Rts>(Generator &gen)
{
    gen.emit_stack_pop_address(Cpu::PC);
    gen.a.inc(Cpu::PC.r16());
    gen.a.ret();
}

template <>
void emit_op<Op::Bcc>(Generator& gen, Address const& operand)
{
    gen.emit_branch_if_clear(StatusFlag::Carry, operand);
}

template <>
void emit_op<Op::Bne>(Generator& gen, Address const& operand)
{
    gen.emit_branch_if_clear(StatusFlag::Zero, operand);
}

template <>
void emit_op<Op::Bpl>(Generator& gen, Address const& operand)
{
    gen.emit_branch_if_clear(StatusFlag::Negative, operand);
}

template <>
void emit_op<Op::Bvc>(Generator& gen, Address const& operand)
{
    gen.emit_branch_if_clear(StatusFlag::Overflow, operand);
}

template <>
void emit_op<Op::Bcs>(Generator& gen, Address const& operand)
{
    gen.emit_branch_if_set(StatusFlag::Carry, operand);
}

template <>
void emit_op<Op::Beq>(Generator& gen, Address const& operand)
{
    gen.emit_branch_if_set(StatusFlag::Zero, operand);
}

template <>
void emit_op<Op::Bmi>(Generator& gen, Address const& operand)
{
    gen.emit_branch_if_set(StatusFlag::Negative, operand);
}

template <>
void emit_op<Op::Bvs>(Generator& gen, Address const& operand)
{
    gen.emit_branch_if_set(StatusFlag::Overflow, operand);
}

template <>
void emit_op<Op::Clc>(Generator& gen)
{
    gen.a.and_(Cpu::Status, ~StatusFlag::Carry);
}

template <>
void emit_op<Op::Cld>(Generator& gen)
{
    gen.a.and_(Cpu::Status, ~StatusFlag::Decimal);
}

template <>
void emit_op<Op::Cli>(Generator& gen)
{
    gen.a.and_(Cpu::Status, ~StatusFlag::InterruptDisable);
}

template <>
void emit_op<Op::Clv>(Generator& gen)
{
    gen.a.and_(Cpu::Status, ~StatusFlag::Overflow);
}

template <>
void emit_op<Op::Sec>(Generator& gen)
{
    gen.a.or_(Cpu::Status, StatusFlag::Carry);
}

template <>
void emit_op<Op::Sed>(Generator& gen)
{
    gen.a.and_(Cpu::Status, StatusFlag::Decimal);
}

template <>
void emit_op<Op::Sei>(Generator& gen)
{
    gen.a.and_(Cpu::Status, StatusFlag::InterruptDisable);
}

template <>
void emit_op<Op::Nop>(Generator& gen)
{
}

template <Op op, AddressingMode addr>
static void emit_instruction(Generator& gen)
{
    if constexpr (addr == AddressingMode::Impl)
    {
        emit_op<op>(gen);
    }
    else
    {
        auto operand = get_operand<addr>(gen);
        emit_op<op>(gen, operand);
    }
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
