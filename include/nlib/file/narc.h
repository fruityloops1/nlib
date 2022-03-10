#pragma once

#include "nlib/binaryreader.h"
#include "nlib/binarywriter.h"
#include "nlib/types.h"

namespace nlib::narc {

namespace detail {
    struct IMG {
        std::vector<u8> data;

        IMG() = default;
        IMG(BinaryReader& reader);
        void write(BinaryWriter& writer) const;
    };
    struct FNT {
        struct DirEntry {
            u32 start = 8;
            u16 fileID = 0;
            u16 parentID = 1;

            DirEntry() = default;
            DirEntry(BinaryReader& reader);
            void write(BinaryWriter& writer) const;
        };
        struct Entry {
            u8 nameLength = 0;
            std::string name;
            u16 dirID;
            bool isEnd = false;
            enum class Type : bool {
                File,
                Directory
            } type
                = Type::File;

            Entry() = default;
            Entry(BinaryReader& reader, Type type, bool isEnd);
            void write(BinaryWriter& writer) const;
        };
        u32 size;
        std::vector<DirEntry> directories;
        std::vector<Entry> entries;
        u32 ffAmount = 0x16;

        FNT() = default;
        FNT(BinaryReader& reader);
        void write(BinaryWriter& writer) const;
    };
    struct FAT {

        struct Entry {
            u32 startOffset;
            u32 endOffset;

            Entry() = default;
            Entry(BinaryReader& reader);
            void write(BinaryWriter& writer) const;
        };

        u32 size;
        u16 fileAmount;
        u16 reserved = 0;

        std::vector<Entry> entries;

        FAT() = default;
        FAT(BinaryReader& reader);
        void write(BinaryWriter& writer) const;
    };
    struct Header {
        u16 byteOrder = 0xFFFE;
        u16 version = 256;
        u32 fileSize;
        u16 headerSize = 0x10;
        u16 dataBlockAmount = 3;

        FAT fat;
        FNT fnt;
        IMG img;

        Header() = default;
        Header(BinaryReader& reader);
        void write(BinaryWriter& writer) const;
    };
} // namespace detail

class Narc {
public:
    struct File {
        std::string name;
        std::vector<u8> data;
    };
    Narc() = default;
    Narc(std::span<const u8> data);
    void addFile(const File& file);
    File& getFile(const std::string& name);
    File& getFile(u16 index);
    inline const std::vector<File>& getFiles() { return mFiles; }

    std::vector<u8> toBinary();

private:
    std::vector<File> mFiles;
};

} // namespace narc
