#pragma once
#include <cstdint>
#include <span>

class ByteStream
{
public:
    explicit ByteStream(std::span<uint8_t> data) : m_data(data) {}

    [[nodiscard]] bool is_end() const { return m_cursor >= m_data.size(); }
    uint8_t next() { return m_data[m_cursor++]; }

private:
    std::span<uint8_t> m_data;
    size_t m_cursor{0};
};
