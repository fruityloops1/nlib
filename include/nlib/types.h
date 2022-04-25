#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>

typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef int64_t s64;
typedef uint64_t u64;

#define BSWAP_STRUCT(T, BSWAP)                            \
    struct b##T {                                         \
        T val;                                            \
        inline void operator=(T val) { this->val = val; } \
        inline operator T() { return val; }               \
        inline T operator~() { return (BSWAP); }          \
    }

BSWAP_STRUCT(s16, (val & 0xFF) << 8 | (val & 0xFF00) >> 8);
BSWAP_STRUCT(u16, (val & 0xFF) << 8 | (val & 0xFF00) >> 8);
BSWAP_STRUCT(s32, ((val & 0x000000FF) << 24) | ((val & 0x0000FF00) << 8) | ((val & 0x00FF0000) >> 8) | (val & 0xFF000000) >> 24);
BSWAP_STRUCT(u32, ((val & 0x000000FF) << 24) | ((val & 0x0000FF00) << 8) | ((val & 0x00FF0000) >> 8) | (val & 0xFF000000) >> 24);
BSWAP_STRUCT(s64, ((val & 0x00000000000000FFULL) << 56) | ((val & 0x000000000000FF00ULL) << 40) | ((val & 0x0000000000FF0000ULL) << 24) | ((val & 0x00000000FF000000ULL) << 8) | ((val & 0x000000FF00000000ULL) >> 8) | ((val & 0x0000FF0000000000ULL) >> 24) | ((val & 0x00FF000000000000ULL) >> 40) | ((val & 0xFF00000000000000ULL) >> 56));
BSWAP_STRUCT(u64, ((val & 0x00000000000000FFULL) << 56) | ((val & 0x000000000000FF00ULL) << 40) | ((val & 0x0000000000FF0000ULL) << 24) | ((val & 0x00000000FF000000ULL) << 8) | ((val & 0x000000FF00000000ULL) >> 8) | ((val & 0x0000FF0000000000ULL) >> 24) | ((val & 0x00FF000000000000ULL) >> 40) | ((val & 0xFF00000000000000ULL) >> 56));

#undef BSWAP_STRUCT

namespace nlib {

class Exception {
    const char* const mMessage { nullptr };

public:
    template <typename... Args>
    Exception(const char* fmt, Args... args)
        : mMessage(new char[snprintf(nullptr, 0, fmt, args...)])
    {
        sprintf((char*)mMessage, fmt, args...);
    }
    ~Exception() { delete[] mMessage; }

    virtual const char* msg() { return mMessage; }

    operator const char*() { return msg(); }
};

} // namespace nlib

#define NLIB_VERIFY(CONDITION, FMT, ...)                                                        \
    {                                                                                           \
        if (!(CONDITION))                                                                       \
            throw ::nlib::Exception("thrown at: %s:%d\n" FMT, __FILE__, __LINE__, __VA_ARGS__); \
    }