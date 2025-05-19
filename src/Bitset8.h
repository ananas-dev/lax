#pragma once

#include <cinttypes>

template<typename Idx>
class Bitset8
{
public:
    Bitset8() : m_bits(0) {}
    explicit Bitset8(uint8_t value) : m_bits(value) {}

    void set(Idx i) { m_bits |= (1 << static_cast<int>(i)); }
    void unset(Idx i) { m_bits &= ~(1 << static_cast<int>(i)); }

    [[nodiscard]] bool test(Idx i) const { return m_bits & (1 << static_cast<int>(i)); }
private:
    uint16_t m_bits;
};
