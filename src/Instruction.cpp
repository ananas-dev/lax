#include "Instruction.h"
#include <array>

static constexpr std::array<Instruction, NUM_OPCODES> opcode_table = {{
#define X(_, op, addressing_mode, size) {Op::op, AddressingMode::addressing_mode, size},
    OPCODES
#undef X
}};

Instruction Instruction::decode(uint8_t opcode) { return opcode_table[opcode]; }
