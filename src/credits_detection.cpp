#include "credits_detection.h"

namespace crde
{
    credits_tc find_credits_timecodes(
            const std::vector< boost::filesystem::path >& biff_folders)
    {
        /** INITIALISATION **/
        /**
         * In this part we just load the folder's biffs in RAM for the
         * further computations
         */
        std::size_t path_count = biff_folders.size();
        std::vector< std::vector< cv::Mat > > sequences;
            sequences.reserve(path_count);
        std::vector< std::string > video_names;
            video_names.reserve(path_count);

        for(const boost::filesystem::path& path : biff_folders)
        {
            /** Basic checks **/
            /** Check if the path lead to a directory **/
            if(!boost::filesystem::is_directory(path))
                throw exception("Path not valid:",
                                "crde::find_credits_timecodes",
                                "Path doesn't lead to a directory:"
                                + path.string());

            /** Check if the path exist **/
            if(!boost::filesystem::exists(path))
                throw exception("Path not valid:",
                                "crde::find_credits_timecodes",
                                "Path doesn't exist" + path.string());

            /** Getting the data **/
            /** Load images in vector**/
            sequences.emplace_back(load_biff_from_dir(path));
            video_names.emplace_back(path.stem().string());
        }

        /** COMPUTATION **/
        /**
         * Here is the main algorithm: We compute over all videos a vector
         * containing some specified metrics (ex: Pix sum, mean and variance...)
         * Then we compare two sequences and find a common subsequence with
         * a threshold (We need a threshold because the credits can vary and may
         * have different compression artefacts).
         * Finaly, we find the sequence with the same threshold in all the video
         */

        /** Computation of the metrics **/
        std::vector< std::vector<uint64_t> > pxsums;
        pxsums.reserve(path_count);

        for(const std::vector< cv::Mat >& sequence : sequences )
            pxsums.emplace_back(pixel_sum_sequence(sequence));

        /** Comparison of two sequences **/
        const uint64_t threshold = 5000;
        std::vector<uint64_t> lcs = utils::longest_common_subseq(pxsums[0],
                                                                 pxsums[1],
                                                                 threshold);

        const std::size_t credits_min_duration = 2;
        credits_tc timecodes;
        if(lcs.size() >= credits_min_duration)
        {
            pxsums.push_back(lcs);
            std::cout << pxsums << std::endl;

            /** Find the timecodes for the first two video **/
            int credits_size = static_cast<int>(lcs.size());
            for(std::size_t i=0; i<path_count; i++)
            {
                std::size_t index = utils::search_thresholded(
                                                        pxsums[i],lcs,threshold);

                if(index != pxsums.size())
                {
                    int start = static_cast<int>(index);
                    timecodes.starts.push_back(start);
                    timecodes.ends.push_back(start + credits_size-1);
                }
                else
                {
                    timecodes.starts.push_back(0);
                    timecodes.ends.push_back(0);
                }
                timecodes.video_names.push_back(video_names[i]);
            }
        }
        else
        {
            std::cerr << "Can't find the credits !" << std::endl;
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
        std::size_t ref_size = ref_name.size();
        std::size_t ref_ind = ref_size-1;
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

        std::size_t number_zeros = (ref_size-1)-ref_ind;

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
        std::size_t seq_size = image_nums.size();
        std::vector<boost::filesystem::path> sorted_paths(seq_size);
        for(std::size_t i=0; i<seq_size; i++)
        {
            std::size_t ind =
                    static_cast<std::size_t>(image_nums[i])-1;
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
                    sum += static_cast<uint64_t>( *image.ptr(x,y)
                                               + *(image.ptr(x,y)+1)
                                               + *(image.ptr(x,y)+2));
                }
            }

            pixel_sums.emplace_back(sum);
        }

        return pixel_sums;
    }

    std::ostream& operator<<(std::ostream& os, const credits_tc& timecodes)
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
