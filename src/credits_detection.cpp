#include "credits_detection.h"

namespace crde
{
    credits_tc find_credits_timecodes(
            const std::vector< boost::filesystem::path >& biff_folders)
    {
        std::vector< boost::filesystem::path >::size_type path_count = biff_folders.size();

        std::vector< std::vector< cv::Mat > > sequences;
            sequences.reserve(path_count);

        for(const boost::filesystem::path& path : biff_folders)
        {
            /** Basic checks **/
            /** Check if the path lead to a directory **/
            if(!boost::filesystem::is_directory(path))
                throw exception("Path not valid:",
                                "crde::find_credits_timecodes",
                                "Path doesn't lead to a directory:" + path.string());

            /** Check if the path exist **/
            if(!boost::filesystem::exists(path))
                throw exception("Path not valid:",
                                "crde::find_credits_timecodes",
                                "Path doesn't exist" + path.string());

            /** Getting the data **/
            /** Load images in vector**/
            sequences.emplace_back(load_biff_from_dir(path));
        }

        /** Now we have to get one number sequences from image, for easy and
         * more understandable comparison process. **/
        std::vector< std::vector<uint64_t> > pix_sum_sequences;
        pix_sum_sequences.reserve(path_count);

        for(std::vector<boost::filesystem::path>::size_type i=0; i<path_count; i++)
            pix_sum_sequences.emplace_back(pixel_sum_sequence(sequences[i]));
        std::cout << pix_sum_sequences << std::endl;

        std::cout << "SUBSEQ" << std::endl;

        /** Find the longest common sequence between two image sequence **/
        std::vector< std::vector<uint64_t> > lcs =
                utils::longest_common_subseq(pix_sum_sequences[0],
                                             pix_sum_sequences[1],
                                             static_cast<uint64_t>(200));

        std::cout << lcs << std::endl;

        /** Finding the starting frame of credits **/
        credits_tc timecodes;
        for(size_t k=0; k<2; k++)
        {
            auto it =
            std::find(pix_sum_sequences[k].begin(), pix_sum_sequences[k].end(), lcs[k][0]);
            int start_tc = static_cast<int>(std::distance(pix_sum_sequences[k].begin(), it));
            int end_tc = static_cast<int>(lcs[k].size()-1);
            timecodes.starts.push_back(start_tc);
            timecodes.ends.push_back(end_tc);
            timecodes.video_names.push_back(" ");
        }

        return timecodes;
    }

    std::vector< cv::Mat > load_biff_from_dir(
            const boost::filesystem::path& path )
    {
        boost::filesystem::directory_iterator dir_it(path);
        boost::filesystem::directory_iterator end_it;
        boost::system::error_code error_code;

        std::vector< cv::Mat > images;
        std::vector< boost::filesystem::path > paths;
        std::vector<int> image_nums;

        /** Determining number of zeros of the sequence **/
        std::string ref_name = dir_it->path().stem().string();
        std::string::size_type ref_size = ref_name.size();
        std::string::size_type ref_ind = ref_size-1;
        while(ref_name[ref_ind] >= '0' && ref_name[ref_ind] <= '9')
        {
            if(ref_ind == 0)
                throw exception("Biff format error",
                                "load_biff_from_dir",
                                "wrong biff name format");
            ref_ind--;
        }
        if(ref_ind == ref_size-1)
            throw exception("Biff format error",
                            "load_biff_from_dir",
                            "No number sequence detected");

        std::string::size_type number_zeros = (ref_size-1)-ref_ind;

        /** Getting all the paths **/
        for(; dir_it != end_it; dir_it.increment(error_code))
        {
            if(error_code)
                throw exception("Boost error occured",
                                "load_image_from_dir",
                                error_code.message());

            std::string file_name = dir_it->path().stem().string();
            paths.push_back(dir_it->path());

            /** We extract the number of the image's path **/
            image_nums.push_back(std::stoi(file_name.substr(file_name.size()-number_zeros,number_zeros)));
        }

        /** Sort the path as sequence **/
        std::vector<int>::size_type seq_size = image_nums.size();
        std::vector<boost::filesystem::path> sorted_paths(seq_size);
        for(std::vector<int>::size_type i=0; i<seq_size; i++)
        {
            std::vector<boost::filesystem::path>::size_type ind =
                    static_cast<std::vector<boost::filesystem::path>::size_type>(image_nums[i])-1;
            sorted_paths[ind] = paths[i];
        }

        /** Load image **/
        images.reserve(seq_size);
        for(const boost::filesystem::path& path: sorted_paths)
        {
            cv::Mat image = cv::imread(path.string(), CV_LOAD_IMAGE_COLOR);
            if(!image.empty())
                images.emplace_back(image);
        }

        return images;
    }

    std::vector<uint64_t> pixel_sum_sequence(
            const std::vector< cv::Mat > images)
    {
        std::vector<uint64_t> pixel_sums;
        pixel_sums.reserve(images.size());

        for(const cv::Mat& image : images)
        {
            uint64_t sum = 0;
            for(int y = 0; y < image.cols; y++)
            {
                for(int x = 0; x < image.rows; x++)
                {
                    sum += static_cast<uint64_t>(
                                *image.ptr(x,y));
                }
            }
            pixel_sums.emplace_back(sum);
        }

        return pixel_sums;
    }

    std::ostream& operator<<(std::ostream& os, const credits_tc& timecodes)
    {
        std::vector<int>::size_type count = timecodes.starts.size();
        const int* start_data = timecodes.starts.data();
        const int* end_data = timecodes.ends.data();

        for(std::vector<int>::size_type i=0; i<count; i++)
        {
            os << "Video: " << timecodes.video_names[i] << '\n'
               << start_data[i] << " --> " << end_data[i] << "\n\n";
        }

        return os;
    }

    std::ostream& operator<<(std::ostream& os,
                const std::vector< std::vector<uint64_t> >& pix_sum_seqs)
    {
        std::vector<std::vector<uint64_t>::size_type> seq_sizes;
        for(const std::vector<uint64_t>& seq : pix_sum_seqs)
            seq_sizes.push_back(seq.size());

        std::vector<uint64_t>::size_type min_size =
                *std::min_element(seq_sizes.begin(), seq_sizes.end());

        for(std::vector<uint64_t>::size_type i=0; i<min_size; i++)
        {
            os << i << ": ";

            for(const std::vector<uint64_t>& seq : pix_sum_seqs)
                os << std::setw(10) << seq[i] << ' ';

            os << '\n';
        }

        return os;
    }
}
