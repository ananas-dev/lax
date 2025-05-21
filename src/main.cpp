#include <asmjit/asmjit.h>
#include <cstdio>
#include <unordered_map>
#include <variant>
#include <vector>

#include "Analysis.h"
#include "ByteStream.h"
#include "Instruction.h"
#include "Rom.h"

using namespace asmjit;

struct CpuState
{
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t s;
    uint8_t status;
};

namespace Cpu
{
    static constexpr auto A = x86::rax;
    static constexpr auto X = x86::rcx;
    static constexpr auto Y = x86::rdx;
    static constexpr auto S = x86::rsi;
    static constexpr auto Status = x86::rdi;
    static constexpr auto PC = x86::r8;
    static constexpr auto Temp = x86::r9;
    static constexpr auto Temp8 = x86::r9b;
} // namespace Cpu

// Signature of the generated function.
typedef void (*Func)(CpuState*);

std::variant<x86::Gpq, Imm> convert_operand(AddressingMode addressing_mode, ByteStream& stream)
{
    switch (addressing_mode)
    {
    case AddressingMode::Impl:
        break;
    case AddressingMode::Imm:
        return Imm(stream.next());
    case AddressingMode::Zpg:
        break;
    case AddressingMode::ZpgX:
        break;
    case AddressingMode::ZpgY:
        break;
    case AddressingMode::Abs:
        break;
    case AddressingMode::AbsX:
        break;
    case AddressingMode::AbsY:
        break;
    case AddressingMode::Ind:
        break;
    case AddressingMode::IndX:
        break;
    case AddressingMode::IndY:
        break;
    case AddressingMode::Rel:
        break;
    case AddressingMode::Acc:
        break;
    }

    return Imm(69);
}

struct InstructionMetadata
{
    bool is_branch_target;
    bool uses_carry_flag;
    bool uses_zero_flag;
    bool uses_overflow_flag;
    bool uses_negative_flag;
};

struct CodeAnalysis
{
    uint16_t entry_point;
    uint16_t pc;
    uint16_t furthest_branch{0};
    bool found_exit_point{false};

    std::unordered_map<uint16_t, InstructionMetadata> instructions{};

    explicit CodeAnalysis(uint16_t entry_point) : entry_point(entry_point) {}
};

class FlowGraph
{
    std::unordered_map<uint16_t, uint16_t> nodes;
};

void jit_trampoline(CpuState* cpu_state, Func f)
{
    asm volatile(
        "movzbl %c[a_off](%[cpu]), %%rax\n\t"
        "movzbl %c[x_off](%[cpu]), %%rcx\n\t"
        "movzbl %c[y_off](%[cpu]), %%rdx\n\t"
        "movzbl %c[status_off](%[cpu]), %%rsi\n\t"

        "call *%[func]\n\t"

        "movb %%rax, %c[a_off](%[cpu])\n\t"
        "movb %%rcx, %c[x_off](%[cpu])\n\t"
        "movb %%rdx, %c[y_off](%[cpu])\n\t"
        "movb %%rsi, %c[status_off](%[cpu])\n\t"
        :
        : [cpu] "r"(cpu_state), [func] "r"(f), [a_off] "i"(offsetof(CpuState, a)), [x_off] "i"(offsetof(CpuState, x)),
          [y_off] "i"(offsetof(CpuState, y)), [status_off] "i"(offsetof(CpuState, status))
        : "rax", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "memory");
}

void emit_update_n(x86::Assembler& a, const x86::Gpq& reg)
{
    // Mask the 7th bit
    a.mov(Cpu::Temp, reg);
    a.and_(Cpu::Temp, 1 << 7);

    // Replace N flag
    a.and_(Cpu::Status, ~(1 << 7));
    a.or_(Cpu::Status, Cpu::Temp);
}

void emit_update_z(x86::Assembler& a, const x86::Gpq& reg)
{
    // Test if reg is zero and store the result in temp
    a.test(reg, reg);
    a.xor_(Cpu::Temp, Cpu::Temp); // Could be avoided ?
    a.setz(Cpu::Temp8);

    // Set the result in the status
    a.shl(Cpu::Temp, 1);
    a.and_(Cpu::Status, ~(1 << 1));
    a.or_(Cpu::Status, Cpu::Temp);
}


struct RuntimeState
{
    std::array<size_t, 0x8000> offsets{};
    std::array<int, 0x8000> function_ids{};
};

int main(int argc, char* argv[])
{
    // Runtime designed for JIT - it holds relocated functions and controls their lifetime.
    JitRuntime rt;

    // Holds code and relocation information during code generation.
    CodeHolder code;

    // Code holder must be initialized before it can be used. The simples way to initialize
    // it is to use 'Environment' from JIT runtime, which matches the target architecture,
    // operating system, ABI, and other important properties.
    code.init(rt.environment(), rt.cpuFeatures());

    x86::Assembler a(&code);

    FileLogger logger(stdout);
    code.setLogger(&logger);

    std::vector<uint8_t> code_bytes = {0xA9, 0x10};

    auto rom = Rom::from_file("nestest.nes");

    uint16_t addr = 0xC000;

    if (!rom.has_value())
    {
        printf("Failed to parse rom!\n");
    }

    RuntimeState rt_state;

    addr -= 0x8000;
    if (addr >= 0x4000)
    {
        addr = addr % 0x4000;
    }

    Analysis analysis(addr, rom->prg_rom);

    analysis.perform();

    // LDA 16
    auto stream = ByteStream(code_bytes);

    auto instruction = Instruction::decode(stream.next());

    // TODO: Generate all the jump labels
    std::unordered_map<uint16_t, Label> labels;

    int pc = 0;

    switch (instruction.op)
    {
    case Op::Lda:
        {
            rt_state.function_ids[pc] = 1;
            rt_state.offsets[pc] = a.offset();

            auto operand = convert_operand(instruction.addressing_mode, stream);
            std::visit([&](auto&& val) { a.mov(Cpu::A, val); }, operand);

            emit_update_n(a, Cpu::A);
            emit_update_z(a, Cpu::A);

            break;
        }
    case Op::Jmp:
        {
            // TODO: Find it
            auto target_address = 0;

            if (instruction.addressing_mode == AddressingMode::Abs)
            {
                auto it = labels.find(target_address);

                if (it != labels.end())
                {
                    auto& label = it->second;
                    a.jmp(label);
                }
                else
                {
                    a.mov(Cpu::PC, target_address);
                    a.ret();
                }
            }
            else
            {
            }
        }

    default:
        exit(1);
    }

    a.ret();

    code.detach(&a);

    Func fn;
    Error err = rt.add(&fn, &code);

    if (err)
    {
        printf("AsmJit failed: %s\n", DebugUtils::errorAsString(err));
        return 1;
    }

    CpuState state{0, 0, 0};

    jit_trampoline(&state, fn);

    printf("State: A=%lu, X=%lu, Y=%lu", state.a, state.x, state.y);

    rt.release(fn);

    return 0;
}
