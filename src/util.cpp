#include "nlib/util.h"
#include <sstream>

std::string nlib::util::readFileString(const std::string& filename)
{
    std::ifstream fstr(filename);
    std::stringstream stream;
    stream << fstr.rdbuf();
    return stream.str();
}
