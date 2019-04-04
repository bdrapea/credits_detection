#pragma once

#include <exception>
#include <string>
#include <ostream>

namespace crde
{
    /**
     * @brief The exception class is a personnalize std::exception
     * for error handling
     */
    class exception : public std::exception
    {
    private:
        /** Name of the error **/
        std::string m_name;

        /** Signature of the function **/
        std::string m_function;

        /** Description of the error **/
        std::string m_desc;

    public:
        /**
         * @brief exception
         * Parameter constructor
         * @param name
         * Specify the name of the error
         * @param function
         * Specify the location of the error (by function's signature)
         * @param desc
         * Specify the description of the error
         */
        exception(const std::string& name,
                  const std::string& function,
                  const std::string& desc);

    friend std::ostream& operator<<(std::ostream& os,
                                    const exception& except);
    };
}
