#pragma once

#include <exception>
#include <string>
#include <ostream>

namespace crde
{
    class exception : public std::exception
    {
    private:
        std::string m_name;
        std::string m_function;
        std::string m_desc;
    public:
        exception(const std::string& name,
                  const std::string& function,
                  const std::string& desc);

    friend std::ostream& operator<<(std::ostream& os,
                                    const exception& except);
    };
}
