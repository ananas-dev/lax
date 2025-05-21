#include "Analysis.h"

typedef void (*AnalysisOpFn)(Analysis&);

Analysis::Analysis(uint16_t entry_point, std::span<uint8_t> memory) :
    entry_point(entry_point), pc(entry_point), last_nz_write(entry_point), last_carry_write(entry_point),
    last_overflow_write(entry_point), furthest_conditional_branch(entry_point), memory(memory)
{
}

void Analysis::write_carry() { last_carry_write = pc; }

void Analysis::write_nz() { last_nz_write = pc; }

void Analysis::write_overflow() { last_overflow_write = pc; }

void Analysis::read_nz() { instructions[last_overflow_write].set(InstructionMetadataFields::ShouldWriteNZ); }

void Analysis::read_carry() { instructions[last_carry_write].set(InstructionMetadataFields::ShouldWriteCarry); }

void Analysis::read_overflow()
{
    instructions[last_overflow_write].set(InstructionMetadataFields::ShouldWriteOverflow);
}

void Analysis::terminal()
{
    if (pc > furthest_conditional_branch)
    {
        found_exit_point = true;
    }
}

void Analysis::branch()
{
    auto offset = static_cast<int8_t>(memory[pc + 1]);
    auto target = static_cast<uint16_t>(pc + static_cast<int16_t>(offset));

    if (target > furthest_conditional_branch)
    {
        furthest_conditional_branch = target;
    }
}

static void op_Adc(Analysis& analysis)
{
    analysis.read_carry();

    analysis.write_nz();
    analysis.write_carry();
    analysis.write_overflow();
}

static void op_And(Analysis& analysis) { analysis.write_nz(); }

static void op_Asl(Analysis& analysis) { analysis.write_nz(); }

static void op_Bcc(Analysis& analysis)
{
    analysis.read_carry();
    analysis.branch();
}

static void op_Bcs(Analysis& analysis)
{
    analysis.read_carry();
    analysis.branch();
}

static void op_Beq(Analysis& analysis)
{
    analysis.read_nz();
    analysis.branch();
}

static void op_Bit(Analysis& analysis)
{
    analysis.read_nz();
    analysis.branch();
}

static void op_Bmi(Analysis& analysis)
{
    analysis.read_nz();
    analysis.branch();
}

static void op_Bne(Analysis& analysis)
{
    analysis.read_nz();
    analysis.branch();
}

static void op_Bpl(Analysis& analysis)
{
    analysis.read_nz();
    analysis.branch();
}

static void op_Bvc(Analysis& analysis)
{
    analysis.read_overflow();
    analysis.branch();
}

static void op_Bvs(Analysis& analysis)
{
    analysis.read_overflow();
    analysis.branch();
}

// TODO
static void op_Brk(Analysis& analysis) { analysis.terminal(); }

static void op_Clc(Analysis& analysis) { analysis.read_carry(); }

// Should not have any effects
static void op_Cld(Analysis& analysis) {}

static void op_Cli(Analysis& analysis) {}

static void op_Clv(Analysis& analysis) { analysis.write_carry(); }

static void op_Cmp(Analysis& analysis)
{
    analysis.write_carry();
    analysis.write_nz();
}

static void op_Cpx(Analysis& analysis) {}
static void op_Cpy(Analysis& analysis) {}
static void op_Dec(Analysis& analysis) {}
static void op_Dex(Analysis& analysis) {}
static void op_Dey(Analysis& analysis) {}
static void op_Eor(Analysis& analysis) {}
static void op_Inc(Analysis& analysis) {}
static void op_Inx(Analysis& analysis) {}
static void op_Iny(Analysis& analysis) {}

static void op_Jmp(Analysis& analysis) { analysis.terminal(); }

static void op_Jsr(Analysis& analysis) { analysis.terminal(); }
static void op_Lda(Analysis& analysis) {}
static void op_Ldx(Analysis& analysis) {}
static void op_Ldy(Analysis& analysis) {}
static void op_Lsr(Analysis& analysis) {}
static void op_Nop(Analysis& analysis) {}
static void op_Ora(Analysis& analysis) {}
static void op_Pha(Analysis& analysis) {}
static void op_Php(Analysis& analysis) {}
static void op_Pla(Analysis& analysis) {}
static void op_Plp(Analysis& analysis) {}
static void op_Rol(Analysis& analysis) {}
static void op_Ror(Analysis& analysis) {}

static void op_Rti(Analysis& analysis) { analysis.terminal(); }

static void op_Rts(Analysis& analysis) { analysis.terminal(); }

static void op_Sbc(Analysis& analysis) {}
static void op_Sec(Analysis& analysis) {}
static void op_Sed(Analysis& analysis) {}
static void op_Sei(Analysis& analysis) {}
static void op_Sta(Analysis& analysis) {}
static void op_Stx(Analysis& analysis) {}
static void op_Sty(Analysis& analysis) {}
static void op_Tax(Analysis& analysis) {}
static void op_Tay(Analysis& analysis) {}
static void op_Tsx(Analysis& analysis) {}
static void op_Txa(Analysis& analysis) {}
static void op_Txs(Analysis& analysis) {}
static void op_Tya(Analysis& analysis) {}
static void op_Ahx(Analysis& analysis) {}
static void op_Alr(Analysis& analysis) {}
static void op_Anc(Analysis& analysis) {}
static void op_Arr(Analysis& analysis) {}
static void op_Axs(Analysis& analysis) {}
static void op_Dcp(Analysis& analysis) {}
static void op_Isc(Analysis& analysis) {}
static void op_Las(Analysis& analysis) {}
static void op_Lax(Analysis& analysis) {}
static void op_Rla(Analysis& analysis) {}
static void op_Rra(Analysis& analysis) {}
static void op_Sax(Analysis& analysis) {}
static void op_Shx(Analysis& analysis) {}
static void op_Shy(Analysis& analysis) {}
static void op_Slo(Analysis& analysis) {}
static void op_Sre(Analysis& analysis) {}
static void op_Stp(Analysis& analysis) {}
static void op_Tas(Analysis& analysis) {}
static void op_Xaa(Analysis& analysis) {}

static constexpr std::array<AnalysisOpFn, NUM_OPCODES> analysis_op_table = {
#define X(i, op, addr, size) op_##op,
    OPCODES
#undef X
};

uint16_t Analysis::perform()
{
    while (!found_exit_point)
    {
        uint8_t opcode = memory[pc];

        auto instruction = Instruction::decode(opcode);

        analysis_op_table[opcode](*this);

        pc += instruction.size;
    }

    return pc - 1;
}
