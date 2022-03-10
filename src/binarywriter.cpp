#include "nlib/binarywriter.h"
#include <cstring>

nlib::BinaryWriter::BinaryWriter(std::endian endian)
    : mEndian(endian)
{
}

void nlib::BinaryWriter::seek(size_t pos)
{
    NLIB_VERIFY(pos < mData.size(), "BinaryWriter::seek: pos larger than size (pos: %zu, size: %zu)", pos, mData.size());
    mPos = pos;
}

void nlib::BinaryWriter::write(std::span<const u8> data)
{
    mData.insert(mData.begin() + mPos, data.begin(), data.end());
    mPos += data.size();
}

void nlib::BinaryWriter::write(const u8* data, size_t len)
{
    mData.insert(mData.begin() + mPos, data, &data[len - 1]);
    mPos += len;
}
