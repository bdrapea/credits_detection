#include "crde_utils.h"

namespace crde
{
    namespace  utils
    {
        std::vector<pic_stats> longest_common_subseq(
                const std::vector<pic_stats>& seq1,
                const std::vector<pic_stats>& seq2,
                const pic_stats threshold)
        {
            std::size_t seq1_size = seq1.size();
            std::size_t seq2_size = seq2.size();
            std::size_t ind = seq1_size;
            std::size_t count = 0;
            std::size_t lcs_ind = 0;
            std::size_t lcs_size = 0;

            for(std::size_t i = 0; i < seq1_size; i++)
            {
                for(std::size_t j = 0; j < seq2_size; j++)
                {
                    if(more_less(seq1[i].mean, seq2[j].mean, threshold.mean)
                    )
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

            auto start = seq1.begin()+
                    static_cast<
                    std::vector<pic_stats>::iterator::difference_type>(lcs_ind);

            auto finish = seq1.begin()+
                    static_cast<
                    std::vector<pic_stats>::iterator::difference_type>(
                        lcs_ind + lcs_size);

            return std::vector<pic_stats>(start,finish);
        }

        std::size_t search_thresholded(const std::vector<pic_stats>& seq,
                                       const std::vector<pic_stats>& subseq,
                                       const pic_stats threshold)
        {
            std::size_t subseq_size = subseq.size();
            std::size_t seq_size = seq.size();

            if(subseq_size > seq_size)
                throw exception("Wrong parameter order",
                                "utils::search_tresholded",
                                "Size of the subsequence must be less than"
                                "the size of the main sequence");

            std::size_t ind = 0;
            for(std::size_t i=0; i<seq_size; i++)
            {
                if(more_less(seq[i].mean,subseq[ind].mean, threshold.mean))
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

        std::ostream& operator<<(std::ostream& os,
                                 const utils::credits_tc& timecodes)
        {
            std::size_t count = timecodes.starts.size();
            const int* start_data = timecodes.starts.data();
            const int* end_data = timecodes.ends.data();

            for(std::size_t i=0; i<count; i++)
            {
                os << "Video: " << timecodes.video_names[i] << '\n'
                   << start_data[i] << " --> " << end_data[i] << "\n\n";
            }

            return os;
        }
    }
}
