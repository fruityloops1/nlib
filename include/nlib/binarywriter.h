#pragma once

#include "nlib/types.h"
#include "nlib/util.h"
#include <bit>
#include <span>
#include <vector>

namespace nlib {

class BinaryWriter {
public:
    BinaryWriter() = default;
    BinaryWriter(std::endian endian);
    template <typename T, bool Swap = true>
    void write(const T& v)
    {
        const u8* data = (u8*)&v;
        if constexpr (Swap) {
            if (mEndian != std::endian::native) {
                T swapped = v;
                nlib::util::swap<sizeof(T)>((u8*)&swapped);
                data = (u8*)&swapped;
                mData.insert(mData.begin() + mPos, data, &data[sizeof(T)]);
                mPos += sizeof(T);
                return;
            }
        }
        mData.insert(mData.begin() + mPos, data, &data[sizeof(T)]);
        mPos += sizeof(T);
    }
    void write(std::span<const u8> data);
    void write(const u8* data, size_t len);

    void seek(size_t pos);
    inline size_t tell() const { return mPos; }
    inline size_t size() const { return mData.size(); }

    inline std::vector<u8> toBinary() const { return mData; }
    inline const u8* data() const { return mData.data(); }

    inline void setEndian(std::endian endian) { mEndian = endian; }
    inline std::endian getEndian() { return mEndian; }

private:
    std::vector<u8> mData;
    size_t mPos = 0;
    std::endian mEndian = std::endian::little;
};

} // namespace nlib
