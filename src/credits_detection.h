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

    /**
     * @brief find_credits_timecodes
     * Main algorithm, it returns the timecodes of the credits for each videos
     * @param biff_folders
     * Paths to video's biff
     * @return
     * Timecodes for each video credits (beginning and ending)
     */
    credits_tc find_credits_timecodes(
            const std::vector< boost::filesystem::path >& biff_folders);

    /**
     * @brief load_biff_from_dir
     * Load a folder filled with biffs into a vector of opencv image
     * @param biff_folder
     * Path to the folder filled with biffs
     * @return
     * Vector of openCV image
     */
    std::vector< cv::Mat > load_biff_from_dir(
            const boost::filesystem::path& biff_folder);

    /**
     * @brief pixel_sum_sequence
     * Compute on a vector of openCV image, a simple sum of each R,G and B
     * Channel value for each frame, it is stored then into a vector of sums
     * @param image_sequence
     * Vector of openCV images
     * @return
     * Vector of sums of pixel
     */
    std::vector< uint64_t > pixel_sum_sequence(
            const std::vector< cv::Mat > image_sequence);

    /**
     * @brief operator <<
     * Print credits_tc properly
     * @param os
     * Buffer for printing purpose
     * @param timecodes
     * Object to print
     * @return
     * Buffer modified
     */
    std::ostream& operator<<(std::ostream& os, const credits_tc& timecodes);

    /**
     * @brief operator <<
     * Print generics vector properly
     * @param os
     * Buffer for printing purpose
     * @param timecodes
     * Object to print
     * @return
     * Buffer modified
     */
    template<typename T>
    std::ostream& operator<<(std::ostream& os,
            const std::vector< std::vector<T> >& pix_sum_seqs)
    {
        std::vector<std::size_t> seq_sizes;
        for(const std::vector<T>& seq : pix_sum_seqs)
            seq_sizes.push_back(seq.size());

        std::size_t max_size =
                *std::max_element(seq_sizes.begin(), seq_sizes.end());

        for(std::size_t i=0; i<max_size; i++)
        {
            os << i << ": ";
            for(std::size_t j=0; j<pix_sum_seqs.size(); j++)
            {
                if(i < pix_sum_seqs[j].size())
                    os << std::setw(10) << pix_sum_seqs[j][i] << ' ';
            }
            os << '\n';
        }

        return os;
    }
}
