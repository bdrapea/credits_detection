#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <iterator>

#include <boost/filesystem.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "crde_exception.h"
#include "crde_utils.h"

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

    std::vector< cv::Mat > load_biff_from_dir(
            const boost::filesystem::path& biff_folder);

    std::vector< uint64_t > pixel_sum_sequence(
            const std::vector< cv::Mat > image_sequence);

    std::ostream& operator<<(std::ostream& os, const credits_tc& timecodes);
    std::ostream& operator<<(std::ostream& os,
            const std::vector< std::vector<uint64_t> >& pix_sum_seqs);
}
