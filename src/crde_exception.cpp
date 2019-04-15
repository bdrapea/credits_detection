#include "crde_exception.h"

namespace crde
{
exception::exception(const std::string&& name,
                     const std::string&& function,
                     const std::string&& desc):
    m_name(name),
    m_function(function),
    m_desc(desc)
{
}

std::ostream& operator<<(std::ostream& os,
                         const exception& except)
{
    os << "===Exception===" << '\n'
       << "Function: " << except.m_function << '\n'
       << "Name: " << except.m_name << '\n'
       << except.m_desc;
    return os;
}
}
