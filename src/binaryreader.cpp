#include "nlib/binaryreader.h"
#include <cstring>

nlib::BinaryReader::BinaryReader(std::span<const u8> data, std::endian endian)
    : mEndian(endian)
{
    mData = data;
}

void nlib::BinaryReader::seek(size_t pos)
{
    NLIB_VERIFY(pos < mData.size(), "BinaryReader::seek: pos larger than size (pos: %zu, size: %zu)", pos, mData.size());
    mPos = pos;
}

std::vector<u8> nlib::BinaryReader::read(size_t amount)
{
    NLIB_VERIFY(mPos + amount < mData.size(), "BinaryReader::read: reached end of file", 0);
    std::vector<u8> data(mData.begin() + mPos, mData.begin() + mPos + amount);
    mPos += amount;
    return data;
}