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
        template<typename T>
        std::vector<T> longest_common_subseq(const std::vector<T>& seq1,
                                             const std::vector<T>& seq2,
                                             const T threshold)
        {
            if(threshold*2 >= std::min(*std::max_element(seq1.begin(),seq1.end()),
                                *std::max_element(seq2.begin(),seq2.end())))
                throw exception("Wrong threshold",
                                "utils::longest_common_seq",
                                "The threshold is too high");

            std::size_t seq1_size = seq1.size();
            std::size_t seq2_size = seq2.size();
            const T* seq1_data = seq1.data();
            const T* seq2_data = seq2.data();
            std::size_t ind = seq1_size;
            std::size_t count = 0;

            std::size_t lcs_ind = 0;
            std::size_t lcs_size = 0;

            for(std::size_t i = 0; i < seq1_size; i++)
            {
                for(std::size_t j = 0; j < seq2_size; j++)
                {
                    if(seq1_data[i] <= seq2_data[j] + threshold
                            && seq1_data[i] >= seq2_data[j] - threshold)
                    {
                        if(ind == seq1_size)
                            ind = i;

                        count++;
                        i++;

                        if(i >= seq1_size)
                            break;
                    }
                    else
                    {
                        if(count > lcs_size)
                        {
                            lcs_ind = ind;
                            lcs_size = count;
                        }

                        ind = seq1_size;
                        count = 0;
                    }
                }
            }

            return std::vector<T>(seq1.begin()+lcs_ind, seq1.begin()+lcs_ind+lcs_size);
        }

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
        template<typename T>
        std::size_t search_thresholded(const std::vector<T>& seq,
                                       const std::vector<T>& subseq,
                                       const T threshold)
        {
            std::size_t subseq_size = subseq.size();
            std::size_t seq_size = seq.size();
            const T* subseq_dat = subseq.data();
            const T* seq_dat = seq.data();

            if(threshold*2 >= *std::max_element(seq.begin(), seq.end()))
                throw exception("Wrong threshold",
                                "utils::search_thresholded",
                                "The threshold is too high");

            if(subseq_size > seq_size)
                throw exception("Wrong parameter order",
                                "utils::search_tresholded",
                                "Size of the subsequence must be less than"
                                "the size of the main sequence");

            std::size_t ind = 0;
            for(std::size_t i=0; i<seq_size; i++)
            {
                if(seq_dat[i] <= subseq_dat[ind]+threshold
                        && seq_dat[i] >= subseq_dat[ind]-threshold)
                {
                    ind++;

                    if(ind == subseq_size)
                        return i-ind+1;
                }
                else
                {
                    ind = 0;
                }
            }

            return seq_size;
        }
    }
}
