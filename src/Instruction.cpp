#include "Instruction.h"
#include <array>

static constexpr std::array<const char*, NUM_OPCODES> string_table = {
#define X(index, op, addressing_mode, size) #op " " #addressing_mode,
    OPCODES
#undef X
};

static constexpr std::array<Instruction, NUM_OPCODES> opcode_table = {{
#define X(_, op, addressing_mode, size) {Op::op, AddressingMode::addressing_mode, size},
    OPCODES
#undef X
}};

Instruction Instruction::decode(uint8_t opcode) { return opcode_table[opcode]; }

const char* Instruction::to_string(uint8_t opcode) { return string_table[opcode]; }
