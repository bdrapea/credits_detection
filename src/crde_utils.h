#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <array>

#include "crde_exception.h"

namespace crde
{
    namespace utils
    {
        /**
         * @brief The credits_tc struct
         * It is a default structure to represent a classic timecode
         */
        struct credits_tc
        {
            /** Frame per seconds **/
            int fps;

            /**< Numbers of the first image of the credits **/
            std::vector<int> starts;

            /**< Numbers of the last image of the credits **/
            std::vector<int> ends;

            /** Name of the videos **/
            std::vector<std::string> video_names;
        };

        /**
         * @brief The pics_stats struct contains all data for BW picture
         */
        struct pic_stats
        {
            std::array<double, 3> mean = {0};

            pic_stats(std::array<double, 3> mn):
                mean(mn){}
        };

        /**
         * @brief longest_common_subseq
         * Simple algorithm to find the longest commmon subsequence between two
         * series, but you can notify a threshold
         * @param subseq1
         * Sequence 1 of oject you want to compare
         * @param subseq2
         * Sequence 2 of object you want to compare
         * @param threshold
         * Threshold for looking similare subseqence
         * @return
         * Common subsequence between the two sequences
         */
        std::vector<pic_stats> longest_common_subseq(
                const std::vector<pic_stats>& seq1,
                const std::vector<pic_stats>& seq2,
                const pic_stats threshold);

        /**
         * @brief search_thresholded
         * Will search a sequence in a bigger one
         * @param seq
         * Sequence of data to look into
         * @param subseq
         * Sequence of data to find
         * @param threshold
         * Threshold value for approximating search
         * @return
         * Index to the first value of the found subsequence, if not it return
         * the size of seq
         */
        std::size_t search_thresholded(const std::vector<pic_stats>& seq,
                                       const std::vector<pic_stats>& subseq,
                                       const pic_stats threshold);

        template <typename T>
        inline bool more_less(const T v1, const T v2, const T threshold)
        {
            if((v1 <= v2 + threshold) && (v1 >= v2 - threshold))
            {
                return true;
            }

            return false;
        }

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
        std::ostream& operator<<(std::ostream& os,
                                 const utils::credits_tc& timecodes);
    }
}
