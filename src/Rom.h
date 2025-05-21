#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

enum class Mirroring
{
    Horizontal,
    Vertical,
    FourScreen
};

struct Rom
{
    static std::optional<Rom> from_file(std::string const& path);

    std::vector<uint8_t> prg_rom;
    std::vector<uint8_t> chr_rom;
    int mapper;
    Mirroring screen_mirroring;

    [[nodiscard]] uint8_t read_prg(uint16_t pc) const;
    [[nodiscard]] uint16_t read_prg_u16(uint16_t pc) const;
};
