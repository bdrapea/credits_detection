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
        template<typename T>
        std::vector<std::vector<T>> longest_common_subseq(const std::vector<T>& subseq1,
                                                           const std::vector<T>& subseq2,
                                                           const T threshold)
        {
            if(threshold >= std::min(*std::max_element(subseq1.begin(),subseq1.end()),
                                *std::max_element(subseq2.begin(),subseq2.end())))
                throw exception("Wrong threshold",
                                 "utils::longest_common_subseq",
                                 "The threshold is too high");


            std::vector<T> common_subseq_i,tmp_i;
            std::vector<T> common_subseq_j,tmp_j;
            for(const T i : subseq1)
            {
                for(const T j :subseq2)
                {
                    if(i <= j+threshold && i >= j-threshold)
                    {
                        tmp_i.push_back(i);
                        tmp_j.push_back(j);
                    }
                    else
                    {
                        size_t tmp_i_size = tmp_i.size();
                        if(tmp_i_size > common_subseq_i.size())
                        {
                            common_subseq_i = tmp_i;
                            common_subseq_j = tmp_j;
                        }

                        if(tmp_i_size == 0)
                        {
                            tmp_i.clear();
                            tmp_j.clear();
                        }
                    }
                }
            }
            return {common_subseq_i, common_subseq_j};
        }
    }
}
