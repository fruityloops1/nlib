#include "nlib/file/narc.h"
#include "oead/yaz0.h"

int main()
{
    std::string hi = "Hello\n";
    std::vector<u8> hiData(hi.begin(), hi.end());

    nlib::narc::Narc n;
    n.addFile({ "test.txt", hiData });
    std::vector<u8> szsData = oead::yaz0::Compress(n.toBinary(), 0, 9);
    nlib::util::writeFile<u8>("test.szs", std::span<u8>(szsData));
    return 0;
}
