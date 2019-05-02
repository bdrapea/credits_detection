#pragma once

#include <fstream>
#include <string>
#include <QString>

#include <boost/filesystem.hpp>

namespace crde
{
namespace gui
{
namespace utils
{
inline std::string load_stylesheet(const boost::filesystem::path& style_path)
{
    std::ifstream stylesheet(style_path.c_str(),
                             std::ios::in|std::ios::binary|std::ios::ate);

    if(!stylesheet.is_open())
        return "";

    std::streamoff style_size = stylesheet.tellg();
    stylesheet.seekg(0,std::ios::beg);
    std::string buff(static_cast<size_t>(style_size),0);
    stylesheet.read(const_cast<char*>(buff.data()),style_size);
    stylesheet.close();
    return buff;
}

inline boost::filesystem::path path_to_sheets(const char* sheets_name = "")
{
    return boost::filesystem::path(__FILE__).
            parent_path().parent_path().append("/sheets/")
            .append(sheets_name);
}
}
}
}
