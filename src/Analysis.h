#pragma once

#include <span>
#include <unordered_map>

#include "Bitset8.h"
#include "Instruction.h"

#include <cinttypes>

enum class InstructionMetadataFields
{
    IsBranchTarget = 0,
    ShouldWriteNZ,
    ShouldWriteCarry,
    ShouldWriteOverflow,
};

struct Analysis
{
    explicit Analysis(uint16_t entry_point);

    uint16_t entry_point;
    uint16_t pc;

    uint16_t last_nz_write;
    uint16_t last_carry_write;
    uint16_t last_overflow_write;

    uint16_t furthest_branch;

    bool found_exit_point{false};

    std::unordered_map<uint16_t, Bitset8<InstructionMetadataFields>> instructions{};

    uint16_t perform(std::span<uint8_t> memory);

    void write_nz();
    void write_carry();
    void write_overflow();

    void read_nz();
    void read_carry();
    void read_overflow();
};

uint16_t analyse_function(std::span<uint8_t> memory, uint16_t entry_point)
{
    uint16_t furthest_branch = 0;
    bool found_exit_point = false;

    uint16_t pc = entry_point;

    while (!found_exit_point)
    {
        auto instruction = Instruction::decode(memory[pc]);


        // if (instruction.op == Op::Jmp || instruction.op == Op::Jsr || instruction.op == Op::Rts ||
        //     instruction.op == Op::Rti || instruction.op == Op::Brk)
        // {
        //     if (pc > furthest_branch)
        //     {
        //         found_exit_point = true;
        //     }
        // }
        // else if (instruction.op == Op::Bcs || instruction.op == Op::Bcc || instruction.op == Op::Beq ||
        //          instruction.op == Op::Bne || instruction.op == Op::Bvs || instruction.op == Op::Bvc ||
        //          instruction.op == Op::Bmi || instruction.op == Op::Bpl)
        // {
        //     auto offset = static_cast<int8_t>(memory[pc + 1]);
        //     auto target = static_cast<uint16_t>(pc + static_cast<int16_t>(offset));
        //
        //     if (target > furthest_branch)
        //     {
        //         furthest_branch = target;
        //     }
        // }

        pc += instruction.size;
    }

    return pc - 1;
}
