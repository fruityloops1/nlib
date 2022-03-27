#include "nlib/file/narc.h"
#include "nlib/binarywriter.h"
#include "nlib/types.h"

namespace nlib::narc {

Narc::Narc(std::span<const u8> data)
{
    BinaryReader reader(data);
    detail::Header header(reader);
    BinaryReader imgReader(header.img.data);

    for (int i = 0; i < header.fat.fileAmount; i++) {
        size_t fileSize = header.fat.entries[i].endOffset - header.fat.entries[i].startOffset;
        imgReader.seek(header.fat.entries[i].startOffset);
        std::vector<u8> fileData = imgReader.read(fileSize);
        mFiles.push_back({ header.fnt.entries[i].name, fileData });
    }
}

void Narc::addFile(const File& file)
{
    for (File& f : mFiles)
        NLIB_VERIFY(file.name != f.name, "nlib::narc::Narc::addFile: duplicate file name (\"%s\")", file.name.c_str());
    mFiles.push_back(file);
}

Narc::File& Narc::getFile(const std::string& name)
{
    for (File& file : mFiles)
        if (file.name == name)
            return file;
    NLIB_VERIFY(false, "nlib::narc::Narc::getFile: file not found", 0);
}

Narc::File& Narc::getFile(u16 index)
{
    NLIB_VERIFY(index < mFiles.size(), "nlib::narc::Narc::getFile: file not found", 0);
    return mFiles[index];
}

std::vector<u8> Narc::toBinary()
{
    BinaryWriter writer;
    detail::Header h;

    h.fat.entries = std::vector<detail::FAT::Entry>(mFiles.size());
    {
        BinaryWriter imgWriter;
        int i = 0;
        for (const File& file : mFiles) {
            size_t startOffset = imgWriter.tell();
            size_t fill = 128 - (startOffset % 128); // pad to 128 bytes
            if (fill == 128)
                fill = 0;
            std::vector<u8> padding(fill, 0xFF);
            imgWriter.write(std::span<const u8>(padding));

            h.fat.entries[i].startOffset = imgWriter.tell();
            imgWriter.write(std::span<const u8>(file.data));
            h.fat.entries[i].endOffset = imgWriter.tell();
            i++;
        }
        h.img.data = imgWriter.toBinary();
    }
    h.fat.size = 12 + h.fat.entries.size() * 8;
    h.fat.fileAmount = h.fat.entries.size();

    h.fnt.directories.push_back(detail::FNT::DirEntry());
    {
        int i = 0;
        for (const File& file : mFiles) {
            detail::FNT::Entry entry;
            entry.name = file.name;
            entry.nameLength = file.name.length();
            h.fnt.entries.push_back(entry);
        }
        detail::FNT::Entry endEntry;
        endEntry.isEnd = true;
        h.fnt.entries.push_back(endEntry);
    }
    size_t fntSize = 8 /* header size */ + 1 /* nameLength */;
    for (const File& file : mFiles)
        fntSize += file.name.length() + 1;
    fntSize += h.fnt.directories.size() * 8;

    h.fnt.size = fntSize;

    h.write(writer);
    return writer.toBinary();
}

} // namespace nlib::narc