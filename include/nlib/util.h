#pragma once

#include "nlib/types.h"
#include <fstream>
#include <span>
#include <vector>

namespace nlib::util {

template <size_t S>
void swap(u8* begin)
{
    u8* end = begin + S - 1;
    u8 tmp;
    while (begin < end) {
        tmp = *begin;
        *begin++ = *end;
        *end-- = tmp;
    }
}

template <>
inline void swap<1>(u8* begin) { }
template <>
inline void swap<2>(u8* begin) { *(u16*)begin = ~(*(bu16*)begin); }
template <>
inline void swap<4>(u8* begin) { *(u32*)begin = ~(*(bu32*)begin); }
template <>
inline void swap<8>(u8* begin) { *(u64*)begin = ~(*(bu64*)begin); }

template <typename T>
std::vector<T> readFile(const std::string& filename)
{
    std::ifstream fstr(filename, std::ios::binary);
    fstr.unsetf(std::ios::skipws);

    std::streampos fileSize;

    fstr.seekg(0, std::ios::end);
    fileSize = fstr.tellg();
    fstr.seekg(0, std::ios::beg);

    NLIB_VERIFY(fileSize % sizeof(T) == 0, "nlib::util::readFile: file size (%zu) not divisible by sizeof(T)", fileSize);

    std::vector<T> data;
    data.resize(fileSize);
    fstr.read((char*)data.data(), fileSize);

    return data;
}

template <typename T>
void writeFile(const std::string& filename, std::span<T> data)
{
    std::ofstream fstr(filename, std::ios::binary);
    fstr.unsetf(std::ios::skipws);
    fstr.write((const char*)data.data(), data.size() * sizeof(T));
}

std::string readFileString(const std::string& filename);

} // namespace nlib::util
