#pragma once

#include <vector>
#include <iostream>

#include <boost/filesystem.hpp>

#include "crde_exception.h"

namespace crde
{
    struct credits_tc
    {
        /**< Numbers of the first image of the credits **/
        std::vector<int> starts;

        /**< Numbers of the last image of the credits **/
        std::vector<int> ends;

        /** Name of the videos **/
        std::vector<std::string> video_names;
    };

    credits_tc find_credits_timecodes(
            const std::vector< boost::filesystem::path >& biff_folders);

    std::ostream& operator<<(std::ostream& os, const credits_tc& timecodes);
}
