#pragma once

#include <cstdint>

constexpr auto NUM_OPCODES = 256;

#define OPCODES                                                                                                        \
    X(0x00, Brk, Impl, 1)                                                                                              \
    X(0x01, Ora, IndX, 2)                                                                                              \
    X(0x02, Stp, Impl, 1)                                                                                              \
    X(0x03, Slo, IndX, 2)                                                                                              \
    X(0x04, Nop, Zpg, 2)                                                                                               \
    X(0x05, Ora, Zpg, 2)                                                                                               \
    X(0x06, Asl, Zpg, 2)                                                                                               \
    X(0x07, Slo, Zpg, 2)                                                                                               \
    X(0x08, Php, Impl, 1)                                                                                              \
    X(0x09, Ora, Imm, 2)                                                                                               \
    X(0x0A, Asl, Acc, 1)                                                                                               \
    X(0x0B, Anc, Imm, 2)                                                                                               \
    X(0x0C, Nop, Abs, 3)                                                                                               \
    X(0x0D, Ora, Abs, 3)                                                                                               \
    X(0x0E, Asl, Abs, 3)                                                                                               \
    X(0x0F, Slo, Abs, 3)                                                                                               \
    X(0x10, Bpl, Rel, 2)                                                                                               \
    X(0x11, Ora, IndY, 2)                                                                                              \
    X(0x12, Stp, Impl, 1)                                                                                              \
    X(0x13, Slo, IndY, 2)                                                                                              \
    X(0x14, Nop, ZpgX, 2)                                                                                              \
    X(0x15, Ora, ZpgX, 2)                                                                                              \
    X(0x16, Asl, ZpgX, 2)                                                                                              \
    X(0x17, Slo, ZpgX, 2)                                                                                              \
    X(0x18, Clc, Impl, 1)                                                                                              \
    X(0x19, Ora, AbsY, 3)                                                                                              \
    X(0x1A, Nop, Impl, 1)                                                                                              \
    X(0x1B, Slo, AbsY, 3)                                                                                              \
    X(0x1C, Nop, AbsX, 3)                                                                                              \
    X(0x1D, Ora, AbsX, 3)                                                                                              \
    X(0x1E, Asl, AbsX, 3)                                                                                              \
    X(0x1F, Slo, AbsX, 3)                                                                                              \
    X(0x20, Jsr, Abs, 3)                                                                                               \
    X(0x21, And, IndX, 2)                                                                                              \
    X(0x22, Stp, Impl, 1)                                                                                              \
    X(0x23, Rla, IndX, 2)                                                                                              \
    X(0x24, Bit, Zpg, 2)                                                                                               \
    X(0x25, And, Zpg, 2)                                                                                               \
    X(0x26, Rol, Zpg, 2)                                                                                               \
    X(0x27, Rla, Zpg, 2)                                                                                               \
    X(0x28, Plp, Impl, 1)                                                                                              \
    X(0x29, And, Imm, 2)                                                                                               \
    X(0x2A, Rol, Acc, 1)                                                                                               \
    X(0x2B, Anc, Imm, 2)                                                                                               \
    X(0x2C, Bit, Abs, 3)                                                                                               \
    X(0x2D, And, Abs, 3)                                                                                               \
    X(0x2E, Rol, Abs, 3)                                                                                               \
    X(0x2F, Rla, Abs, 3)                                                                                               \
    X(0x30, Bmi, Rel, 2)                                                                                               \
    X(0x31, And, IndY, 2)                                                                                              \
    X(0x32, Stp, Impl, 1)                                                                                              \
    X(0x33, Rla, IndY, 2)                                                                                              \
    X(0x34, Nop, ZpgX, 2)                                                                                              \
    X(0x35, And, ZpgX, 2)                                                                                              \
    X(0x36, Rol, ZpgX, 2)                                                                                              \
    X(0x37, Rla, ZpgX, 2)                                                                                              \
    X(0x38, Sec, Impl, 1)                                                                                              \
    X(0x39, And, AbsY, 3)                                                                                              \
    X(0x3A, Nop, Impl, 1)                                                                                              \
    X(0x3B, Rla, AbsY, 3)                                                                                              \
    X(0x3C, Nop, AbsX, 3)                                                                                              \
    X(0x3D, And, AbsX, 3)                                                                                              \
    X(0x3E, Rol, AbsX, 3)                                                                                              \
    X(0x3F, Rla, AbsX, 3)                                                                                              \
    X(0x40, Rti, Impl, 1)                                                                                              \
    X(0x41, Eor, IndX, 2)                                                                                              \
    X(0x42, Stp, Impl, 1)                                                                                              \
    X(0x43, Sre, IndX, 2)                                                                                              \
    X(0x44, Nop, Zpg, 2)                                                                                               \
    X(0x45, Eor, Zpg, 2)                                                                                               \
    X(0x46, Lsr, Zpg, 2)                                                                                               \
    X(0x47, Sre, Zpg, 2)                                                                                               \
    X(0x48, Pha, Impl, 1)                                                                                              \
    X(0x49, Eor, Imm, 2)                                                                                               \
    X(0x4A, Lsr, Acc, 1)                                                                                               \
    X(0x4B, Alr, Imm, 2)                                                                                               \
    X(0x4C, Jmp, Abs, 3)                                                                                               \
    X(0x4D, Eor, Abs, 3)                                                                                               \
    X(0x4E, Lsr, Abs, 3)                                                                                               \
    X(0x4F, Sre, Abs, 3)                                                                                               \
    X(0x50, Bvc, Rel, 2)                                                                                               \
    X(0x51, Eor, IndY, 2)                                                                                              \
    X(0x52, Stp, Impl, 1)                                                                                              \
    X(0x53, Sre, IndY, 2)                                                                                              \
    X(0x54, Nop, ZpgX, 2)                                                                                              \
    X(0x55, Eor, ZpgX, 2)                                                                                              \
    X(0x56, Lsr, ZpgX, 2)                                                                                              \
    X(0x57, Sre, ZpgX, 2)                                                                                              \
    X(0x58, Cli, Impl, 1)                                                                                              \
    X(0x59, Eor, AbsY, 3)                                                                                              \
    X(0x5A, Nop, Impl, 1)                                                                                              \
    X(0x5B, Sre, AbsY, 3)                                                                                              \
    X(0x5C, Nop, AbsX, 3)                                                                                              \
    X(0x5D, Eor, AbsX, 3)                                                                                              \
    X(0x5E, Lsr, AbsX, 3)                                                                                              \
    X(0x5F, Sre, AbsX, 3)                                                                                              \
    X(0x60, Rts, Impl, 1)                                                                                              \
    X(0x61, Adc, IndX, 2)                                                                                              \
    X(0x62, Stp, Impl, 1)                                                                                              \
    X(0x63, Rra, IndX, 2)                                                                                              \
    X(0x64, Nop, Zpg, 2)                                                                                               \
    X(0x65, Adc, Zpg, 2)                                                                                               \
    X(0x66, Ror, Zpg, 2)                                                                                               \
    X(0x67, Rra, Zpg, 2)                                                                                               \
    X(0x68, Pla, Impl, 1)                                                                                              \
    X(0x69, Adc, Imm, 2)                                                                                               \
    X(0x6A, Ror, Acc, 1)                                                                                               \
    X(0x6B, Arr, Imm, 2)                                                                                               \
    X(0x6C, Jmp, Ind, 3)                                                                                               \
    X(0x6D, Adc, Abs, 3)                                                                                               \
    X(0x6E, Ror, Abs, 3)                                                                                               \
    X(0x6F, Rra, Abs, 3)                                                                                               \
    X(0x70, Bvs, Rel, 2)                                                                                               \
    X(0x71, Adc, IndY, 2)                                                                                              \
    X(0x72, Stp, Impl, 1)                                                                                              \
    X(0x73, Rra, IndY, 2)                                                                                              \
    X(0x74, Nop, ZpgX, 2)                                                                                              \
    X(0x75, Adc, ZpgX, 2)                                                                                              \
    X(0x76, Ror, ZpgX, 2)                                                                                              \
    X(0x77, Rra, ZpgX, 2)                                                                                              \
    X(0x78, Sei, Impl, 1)                                                                                              \
    X(0x79, Adc, AbsY, 3)                                                                                              \
    X(0x7A, Nop, Impl, 1)                                                                                              \
    X(0x7B, Rra, AbsY, 3)                                                                                              \
    X(0x7C, Nop, AbsX, 3)                                                                                              \
    X(0x7D, Adc, AbsX, 3)                                                                                              \
    X(0x7E, Ror, AbsX, 3)                                                                                              \
    X(0x7F, Rra, AbsX, 3)                                                                                              \
    X(0x80, Nop, Imm, 2)                                                                                               \
    X(0x81, Sta, IndX, 2)                                                                                              \
    X(0x82, Nop, Imm, 2)                                                                                               \
    X(0x83, Sax, IndX, 2)                                                                                              \
    X(0x84, Sty, Zpg, 2)                                                                                               \
    X(0x85, Sta, Zpg, 2)                                                                                               \
    X(0x86, Stx, Zpg, 2)                                                                                               \
    X(0x87, Sax, Zpg, 2)                                                                                               \
    X(0x88, Dey, Impl, 1)                                                                                              \
    X(0x89, Nop, Imm, 2)                                                                                               \
    X(0x8A, Txa, Impl, 1)                                                                                              \
    X(0x8B, Xaa, Imm, 2)                                                                                               \
    X(0x8C, Sty, Abs, 3)                                                                                               \
    X(0x8D, Sta, Abs, 3)                                                                                               \
    X(0x8E, Stx, Abs, 3)                                                                                               \
    X(0x8F, Sax, Abs, 3)                                                                                               \
    X(0x90, Bcc, Rel, 2)                                                                                               \
    X(0x91, Sta, IndY, 2)                                                                                              \
    X(0x92, Stp, Impl, 1)                                                                                              \
    X(0x93, Ahx, IndY, 2)                                                                                              \
    X(0x94, Sty, ZpgX, 2)                                                                                              \
    X(0x95, Sta, ZpgX, 2)                                                                                              \
    X(0x96, Stx, ZpgY, 2)                                                                                              \
    X(0x97, Sax, ZpgY, 2)                                                                                              \
    X(0x98, Tya, Impl, 1)                                                                                              \
    X(0x99, Sta, AbsY, 3)                                                                                              \
    X(0x9A, Txs, Impl, 1)                                                                                              \
    X(0x9B, Tas, AbsY, 3)                                                                                              \
    X(0x9C, Shy, AbsX, 3)                                                                                              \
    X(0x9D, Sta, AbsX, 3)                                                                                              \
    X(0x9E, Shx, AbsY, 3)                                                                                              \
    X(0x9F, Ahx, AbsY, 3)                                                                                              \
    X(0xA0, Ldy, Imm, 2)                                                                                               \
    X(0xA1, Lda, IndX, 2)                                                                                              \
    X(0xA2, Ldx, Imm, 2)                                                                                               \
    X(0xA3, Lax, IndX, 2)                                                                                              \
    X(0xA4, Ldy, Zpg, 2)                                                                                               \
    X(0xA5, Lda, Zpg, 2)                                                                                               \
    X(0xA6, Ldx, Zpg, 2)                                                                                               \
    X(0xA7, Lax, Zpg, 2)                                                                                               \
    X(0xA8, Tay, Impl, 1)                                                                                              \
    X(0xA9, Lda, Imm, 2)                                                                                               \
    X(0xAA, Tax, Impl, 1)                                                                                              \
    X(0xAB, Lax, Imm, 2)                                                                                               \
    X(0xAC, Ldy, Abs, 3)                                                                                               \
    X(0xAD, Lda, Abs, 3)                                                                                               \
    X(0xAE, Ldx, Abs, 3)                                                                                               \
    X(0xAF, Lax, Abs, 3)                                                                                               \
    X(0xB0, Bcs, Rel, 2)                                                                                               \
    X(0xB1, Lda, IndY, 2)                                                                                              \
    X(0xB2, Stp, Impl, 1)                                                                                              \
    X(0xB3, Lax, IndY, 2)                                                                                              \
    X(0xB4, Ldy, ZpgX, 2)                                                                                              \
    X(0xB5, Lda, ZpgX, 2)                                                                                              \
    X(0xB6, Ldx, ZpgY, 2)                                                                                              \
    X(0xB7, Lax, ZpgY, 2)                                                                                              \
    X(0xB8, Clv, Impl, 1)                                                                                              \
    X(0xB9, Lda, AbsY, 3)                                                                                              \
    X(0xBA, Tsx, Impl, 1)                                                                                              \
    X(0xBB, Las, AbsY, 3)                                                                                              \
    X(0xBC, Ldy, AbsX, 3)                                                                                              \
    X(0xBD, Lda, AbsX, 3)                                                                                              \
    X(0xBE, Ldx, AbsY, 3)                                                                                              \
    X(0xBF, Lax, AbsY, 3)                                                                                              \
    X(0xC0, Cpy, Imm, 2)                                                                                               \
    X(0xC1, Cmp, IndX, 2)                                                                                              \
    X(0xC2, Nop, Imm, 2)                                                                                               \
    X(0xC3, Dcp, IndX, 2)                                                                                              \
    X(0xC4, Cpy, Zpg, 2)                                                                                               \
    X(0xC5, Cmp, Zpg, 2)                                                                                               \
    X(0xC6, Dec, Zpg, 2)                                                                                               \
    X(0xC7, Dcp, Zpg, 2)                                                                                               \
    X(0xC8, Iny, Impl, 1)                                                                                              \
    X(0xC9, Cmp, Imm, 2)                                                                                               \
    X(0xCA, Dex, Impl, 1)                                                                                              \
    X(0xCB, Axs, Imm, 2)                                                                                               \
    X(0xCC, Cpy, Abs, 3)                                                                                               \
    X(0xCD, Cmp, Abs, 3)                                                                                               \
    X(0xCE, Dec, Abs, 3)                                                                                               \
    X(0xCF, Dcp, Abs, 3)                                                                                               \
    X(0xD0, Bne, Rel, 2)                                                                                               \
    X(0xD1, Cmp, IndY, 2)                                                                                              \
    X(0xD2, Stp, Impl, 1)                                                                                              \
    X(0xD3, Dcp, IndY, 2)                                                                                              \
    X(0xD4, Nop, ZpgX, 2)                                                                                              \
    X(0xD5, Cmp, ZpgX, 2)                                                                                              \
    X(0xD6, Dec, ZpgX, 2)                                                                                              \
    X(0xD7, Dcp, ZpgX, 2)                                                                                              \
    X(0xD8, Cld, Impl, 1)                                                                                              \
    X(0xD9, Cmp, AbsY, 3)                                                                                              \
    X(0xDA, Nop, Impl, 1)                                                                                              \
    X(0xDB, Dcp, AbsY, 3)                                                                                              \
    X(0xDC, Nop, AbsX, 3)                                                                                              \
    X(0xDD, Cmp, AbsX, 3)                                                                                              \
    X(0xDE, Dec, AbsX, 3)                                                                                              \
    X(0xDF, Dcp, AbsX, 3)                                                                                              \
    X(0xE0, Cpx, Imm, 2)                                                                                               \
    X(0xE1, Sbc, IndX, 2)                                                                                              \
    X(0xE2, Nop, Imm, 2)                                                                                               \
    X(0xE3, Isc, IndX, 2)                                                                                              \
    X(0xE4, Cpx, Zpg, 2)                                                                                               \
    X(0xE5, Sbc, Zpg, 2)                                                                                               \
    X(0xE6, Inc, Zpg, 2)                                                                                               \
    X(0xE7, Isc, Zpg, 2)                                                                                               \
    X(0xE8, Inx, Impl, 1)                                                                                              \
    X(0xE9, Sbc, Imm, 2)                                                                                               \
    X(0xEA, Nop, Impl, 1)                                                                                              \
    X(0xEB, Sbc, Imm, 2)                                                                                               \
    X(0xEC, Cpx, Abs, 3)                                                                                               \
    X(0xED, Sbc, Abs, 3)                                                                                               \
    X(0xEE, Inc, Abs, 3)                                                                                               \
    X(0xEF, Isc, Abs, 3)                                                                                               \
    X(0xF0, Beq, Rel, 2)                                                                                               \
    X(0xF1, Sbc, IndY, 2)                                                                                              \
    X(0xF2, Stp, Impl, 1)                                                                                              \
    X(0xF3, Isc, IndY, 2)                                                                                              \
    X(0xF4, Nop, ZpgX, 2)                                                                                              \
    X(0xF5, Sbc, ZpgX, 2)                                                                                              \
    X(0xF6, Inc, ZpgX, 2)                                                                                              \
    X(0xF7, Isc, ZpgX, 2)                                                                                              \
    X(0xF8, Sed, Impl, 1)                                                                                              \
    X(0xF9, Sbc, AbsY, 3)                                                                                              \
    X(0xFA, Nop, Impl, 1)                                                                                              \
    X(0xFB, Isc, AbsY, 3)                                                                                              \
    X(0xFC, Nop, AbsX, 3)                                                                                              \
    X(0xFD, Sbc, AbsX, 3)                                                                                              \
    X(0xFE, Inc, AbsX, 3)                                                                                              \
    X(0xFF, Isc, AbsX, 3)

enum class Op
{
    Adc,
    And,
    Asl,
    Bcc,
    Bcs,
    Beq,
    Bit,
    Bmi,
    Bne,
    Bpl,
    Brk,
    Bvc,
    Bvs,
    Clc,
    Cld,
    Cli,
    Clv,
    Cmp,
    Cpx,
    Cpy,
    Dec,
    Dex,
    Dey,
    Eor,
    Inc,
    Inx,
    Iny,
    Jmp,
    Jsr,
    Lda,
    Ldx,
    Ldy,
    Lsr,
    Nop,
    Ora,
    Pha,
    Php,
    Pla,
    Plp,
    Rol,
    Ror,
    Rti,
    Rts,
    Sbc,
    Sec,
    Sed,
    Sei,
    Sta,
    Stx,
    Sty,
    Tax,
    Tay,
    Tsx,
    Txa,
    Txs,
    Tya,

    // Illegal/Unofficial opcodes
    Ahx,
    Alr,
    Anc,
    Arr,
    Axs,
    Dcp,
    Isc,
    Las,
    Lax,
    Rla,
    Rra,
    Sax,
    Shx,
    Shy,
    Slo,
    Sre,
    Stp,
    Tas,
    Xaa
};

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

enum class AddressingModeCategory
{
    None = 0,
    Value,
    Register,
    Address,
};

AddressingModeCategory addressing_mode_category(AddressingMode addressing_mode);

struct Instruction
{
    Op op;
    AddressingMode addressing_mode;
    uint16_t size;

    static Instruction decode(uint8_t opcode);
};
