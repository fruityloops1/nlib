#pragma once

#include "nlib/types.h"
#include "nlib/util.h"
#include <bit>
#include <span>
#include <vector>

namespace nlib {

class BinaryReader {
public:
    BinaryReader(std::span<const u8> data, std::endian endian = std::endian::little);

    template <typename T, bool Swap = true>
    T read()
    {
        NLIB_VERIFY(mPos + sizeof(T) < mData.size(), "BinaryReader::read: reached end of file", 0);
        T& value = *(T*)&mData[mPos];
        mPos += sizeof(T);

        if constexpr (Swap) {
            if (mEndian != std::endian::native) {
                T valueSwapped = value;
                nlib::util::swap<sizeof(T)>((u8*)&valueSwapped);
                return valueSwapped;
            }
        }

        return value;
    }
    std::vector<u8> read(size_t amount);

    void seek(size_t pos);
    inline size_t tell() const { return mPos; }
    inline size_t size() const { return mData.size(); }
    inline void setEndian(std::endian endian) { mEndian = endian; }
    inline std::endian getEndian() { return mEndian; }

private:
    std::span<const u8> mData;
    size_t mPos = 0;
    std::endian mEndian = std::endian::little;
};

} // namespace nlib
