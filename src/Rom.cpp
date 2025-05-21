#include "Rom.h"

#include <array>
#include <cstdio>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

constexpr size_t PRG_ROM_PAGE_SIZE = 0x4000;
constexpr size_t CHR_ROM_PAGE_SIZE = 0x2000;

std::optional<Rom> Rom::from_file(std::string const& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
        return {};

    uint8_t header[16];
    if (!file.read(reinterpret_cast<char*>(header), 16))
        return {};

    if (!(header[0] == 0x4E && header[1] == 0x45 && header[2] == 0x53 && header[3] == 0x1A))
        return {};

    int mapper = (header[7] & 0xF0) | (header[6] >> 4);
    int ines_ver = (header[7] >> 2) & 0b11;

    if (ines_ver != 0)
        return {};

    bool four_screen = header[6] & 0x08;
    bool vertical_mirroring = header[6] & 0x01;

    Mirroring mirroring = four_screen ? Mirroring::FourScreen
        : vertical_mirroring          ? Mirroring::Vertical
                                      : Mirroring::Horizontal;

    size_t prg_rom_size = header[4] * PRG_ROM_PAGE_SIZE;
    size_t chr_rom_size = header[5] * CHR_ROM_PAGE_SIZE;

    bool skip_trainer = header[6] & 0x04;
    size_t trainer_size = skip_trainer ? 512 : 0;

    // Skip trainer if needed
    if (skip_trainer)
        file.seekg(512, std::ios::cur);

    std::vector<uint8_t> prg_rom(prg_rom_size);
    if (!file.read(reinterpret_cast<char*>(prg_rom.data()), prg_rom_size))
        return {};

    std::vector<uint8_t> chr_rom(chr_rom_size);
    if (!file.read(reinterpret_cast<char*>(chr_rom.data()), chr_rom_size))
        return {};

    return Rom{
        .prg_rom = std::move(prg_rom), .chr_rom = std::move(chr_rom), .mapper = mapper, .screen_mirroring = mirroring};
}

uint8_t Rom::read_prg(uint16_t pc) const
{
    if (pc < 0x8000 || pc >= 0xFFFA)
    {
        printf("Out of bounds prg read!\n");
        return 0;
    }

    return prg_rom[pc - 0x8000];
}

uint16_t Rom::read_prg_u16(uint16_t pc) const
{
    uint16_t low = read_prg(pc);
    uint16_t high = read_prg(pc + 1);

    return (high << 8) | low;
}
