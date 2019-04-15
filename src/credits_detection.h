#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <iterator>
#include <fstream>
#include <sstream>

#include <boost/filesystem.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "crde_exception.h"
#include "crde_utils.h"

namespace crde
{
/**
 * @brief find_credits_timecodes
 * Main algorithm, it returns the timecodes of the credits for each videos
 * @param biff_folders
 * Paths to video's biff
 * @return
 * Timecodes for each video credits (beginning and ending)
 */
utils::credits_tc find_credits_timecodes(
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
 * @brief find_longest_common_sequence
 * This algorithm will find the common sequence of two vector of images
 * @param seq1
 * First sequence of images
 * @param seq2
 * Second sequence of images
 * @param sequence_begins
 * Begining of the first common sequence
 * @param sequence_length
 * Size of the first sequence's common sequence
 * @return
 * True if common sequence is found
 */
bool find_longest_common_sequence(
    const std::vector< cv::Mat >& seq1,
    const std::vector< cv::Mat >& seq2,
    std::size_t* sequence_begins,
    std::size_t* sequence_length);

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
