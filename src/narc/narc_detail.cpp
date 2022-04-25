#include "nlib/binaryreader.h"
#include "nlib/binarywriter.h"
#include "nlib/file/narc.h"
#include <bits/types/FILE.h>

namespace nlib::narc::detail {

Header::Header(BinaryReader& reader)
{
    union {
        u32 val;
        char str[4];
    } magic { reader.read<u32>() };
    NLIB_VERIFY(magic.str[0] == 'N' && magic.str[1] == 'A' && magic.str[2] == 'R' && magic.str[3] == 'C',
        "nlib::narc::detail::Header::Header: invalid magic", 0);

    byteOrder = reader.read<u16>();
    version = reader.read<u16>();
    fileSize = reader.read<u32>();
    headerSize = reader.read<u16>();
    dataBlockAmount = reader.read<u16>();

    fat = FAT(reader);
    fnt = FNT(reader);
    img = IMG(reader);
}

void Header::write(BinaryWriter& writer) const
{
    constexpr union {
        char str[4];
        u32 val;
    } magic { 'N', 'A', 'R', 'C' };
    writer.write(magic.val);
    writer.write<u16>(0xFFFE);
    writer.write(version);
    writer.write(fileSize);
    writer.write(headerSize);
    writer.write(dataBlockAmount);
    fat.write(writer);
    fnt.write(writer);
    img.write(writer);
}

FAT::FAT(BinaryReader& reader)
{
    size_t startPos = reader.tell();

    NLIB_VERIFY(reader.read<u32>() == 0x46415442 /* 'FATB' */, "nlib::narc::detail::FAT::FAT: invalid magic", 0);
    size = reader.read<u32>();
    fileAmount = reader.read<u16>();
    reserved = reader.read<u16>();

    for (int i = 0; i < fileAmount; i++)
        entries.push_back(Entry(reader));

    reader.seek(startPos + size);
}

void FAT::write(BinaryWriter& writer) const
{
    writer.write<u32>(0x46415442); // 'FATB'
    writer.write(size);
    writer.write(fileAmount);
    writer.write(reserved);

    for (const Entry& entry : entries)
        entry.write(writer);
}

FAT::Entry::Entry(BinaryReader& reader)
{
    startOffset = reader.read<u32>();
    endOffset = reader.read<u32>();
}

void FAT::Entry::write(BinaryWriter& writer) const
{
    writer.write(startOffset);
    writer.write(endOffset);
}

FNT::FNT(BinaryReader& reader)
{
    size_t startPos = reader.tell();
    NLIB_VERIFY(reader.read<u32>() == 0x464E5442 /* 'FNTB' */, "nlib::narc::detail::FNT::FNT: invalid magic", 0);
    size = reader.read<u32>();

    directories.push_back(DirEntry(reader));
    for (int i = 0; i < directories[0].parentID - 1; i++)
        directories.push_back(DirEntry(reader));

    int endOfDirectory = 0;
    while (endOfDirectory < directories[0].parentID) {
        u8 nameLength = reader.read<u8>();
        reader.seek(reader.tell() - 1);
        if (nameLength == 0) {
            entries.push_back(Entry(reader, Entry::Type::File, true));
            endOfDirectory++;
        } else if (nameLength < 128)
            entries.push_back(Entry(reader, Entry::Type::File, false));
        else
            entries.push_back(Entry(reader, Entry::Type::Directory, false));
    }
}

void FNT::write(BinaryWriter& writer) const
{
    writer.write<u32>(0x464E5442); // 'FNTB'

    size_t startOffset = writer.tell() + size - 4;
    size_t fill = 128 - (startOffset % 128) - 8; // pad to 128 bytes
    if (fill == 128)
        fill = 0;
    std::vector<u8> padding(fill, 0xFF);

    writer.write<u32>(size + fill);
    for (const DirEntry& dirEntry : directories)
        dirEntry.write(writer);
    for (const Entry& entry : entries)
        entry.write(writer);
}

FNT::DirEntry::DirEntry(BinaryReader& reader)
{
    start = reader.read<u32>();
    fileID = reader.read<u16>();
    parentID = reader.read<u16>();
}

void FNT::DirEntry::write(BinaryWriter& writer) const
{
    writer.write(start);
    writer.write(fileID);
    writer.write(parentID);
}

FNT::Entry::Entry(BinaryReader& reader, Type type, bool isEnd)
    : type(type)
    , isEnd(isEnd)
{
    nameLength = reader.read<u8>();
    if (isEnd)
        return;

    if (type == Type::File) {
        std::vector<u8> nameData = reader.read(nameLength);
        name = std::string(nameData.begin(), nameData.end());
    } else if (type == Type::Directory) {
        std::vector<u8> nameData = reader.read(nameLength & 0x7F);
        name = std::string(nameData.begin(), nameData.end());
        dirID = reader.read<u16>();
    }
}

void FNT::Entry::write(BinaryWriter& writer) const
{
    writer.write(nameLength);
    if (isEnd)
        return;

    if (type == Type::File) {
        std::vector<u8> nameData(name.cbegin(), name.cend());
        writer.write(std::span<const u8>(nameData));
    } else if (type == Type::Directory) {
        std::vector<u8> nameData(name.cbegin(), name.cend());
        writer.write(std::span<const u8>(nameData));
        writer.write<u16>(dirID);
    }
}

IMG::IMG(BinaryReader& reader)
{
    size_t startOffset = reader.tell();
    size_t fill = 128 - (startOffset % 128); // pad to 128 bytes
    if (fill == 128)
        fill = 0;
    startOffset += fill;
    reader.seek(startOffset - 8);

    NLIB_VERIFY(reader.read<u32>() == 0x46494D47 /* 'FIMG' */, "nlib::narc::detail::IMG::IMG: invalid magic", 0);
    u32 size = reader.read<u32>();
    size_t o = reader.tell();
    data = reader.read(size - 9);
    reader.seek(o);
}

void IMG::write(BinaryWriter& writer) const
{
    size_t startOffset = writer.tell();
    size_t fill = 128 - (startOffset % 128); // pad to 128 bytes
    if (fill == 128)
        fill = 0;
    std::vector<u8> padding(fill - 8, 0xFF);
    writer.write(std::span<const u8>(padding));

    writer.write<u32>(0x46494D47); // 'FIMG'

    startOffset = writer.tell() + 4 + data.size();
    fill = 128 - (startOffset % 128); // pad to 128 bytes
    if (fill == 128)
        fill = 0;
    padding = std::vector<u8>(fill, 0xFF);

    writer.write<u32>(data.size() + fill + 8);
    writer.write(std::span<const u8>(data));
    writer.write(std::span<const u8>(padding));
}

} // namespace nlib::narc::detail