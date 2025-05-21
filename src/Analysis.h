#pragma once

#include <span>
#include <unordered_map>

#include "Bitset8.h"
#include "Instruction.h"

#include <cinttypes>

#include "Rom.h"

enum class InstructionMetadataFields
{
    IsBranchTarget = 0,
    ShouldWriteNZ,
    ShouldWriteCarry,
    ShouldWriteOverflow,
};

struct Analysis
{
    explicit Analysis(uint16_t entry_point, Rom rom);

    Rom rom;

    uint16_t entry_point;
    uint16_t pc;

    uint16_t last_nz_write;
    uint16_t last_carry_write;
    uint16_t last_overflow_write;

    uint16_t furthest_conditional_branch;

    bool found_exit_point{false};

    std::unordered_map<uint16_t, Bitset8<InstructionMetadataFields>> instructions{};

    uint16_t perform();

    void write_nz();
    void write_carry();
    void write_overflow();

    void read_nz();
    void read_carry();
    void read_overflow();

    void terminal();
    void branch();
};
