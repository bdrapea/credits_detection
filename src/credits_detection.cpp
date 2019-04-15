#include "credits_detection.h"

namespace crde
{
utils::credits_tc find_credits_timecodes(
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
        //Check if the path lead to a directory
        if(!boost::filesystem::is_directory(path))
            throw exception("Path not valid:",
                            "crde::find_credits_timecodes",
                            "Path doesn't lead to a directory:"
                            + path.string());

        //Check if the path exist
        if(!boost::filesystem::exists(path))
            throw exception("Path not valid:",
                            "crde::find_credits_timecodes",
                            "Path doesn't exist" + path.string());

        /** Getting the data **/
        //Load images in vector
        sequences.emplace_back(load_biff_from_dir(path));
        video_names.emplace_back(path.filename().c_str());
    }

    /** COMPUTATION **/
    bool credits_found = true;
    utils::credits_tc timecodes;
    timecodes.video_names = video_names;
    timecodes.starts.assign(path_count,0);
    timecodes.ends.assign(path_count,0);
    std::size_t credits_length;
    std::size_t j=0;

    for(; j<path_count-1; j++)
    {
        credits_length = 0;
        credits_found *= find_longest_common_sequence(
                                                sequences[j],
                                                sequences[j+1],
                                                &timecodes.starts[j],
                                                &timecodes.starts[j+1],
                                                &credits_length);

        timecodes.ends[j] = timecodes.starts[j] + credits_length;
    }

    timecodes.ends[j] = timecodes.starts[j] + credits_length;

    if(!credits_found)
    {
        std::cerr << "Can't find credits" << std::endl;
        return utils::credits_tc();
    }

    return timecodes;
}

bool find_longest_common_sequence(
    const std::vector< cv::Mat >& seq1,
    const std::vector< cv::Mat >& seq2,
    std::size_t* sequence1_begin,
    std::size_t* sequence2_begin,
    std::size_t* sequence_length)
{
    std::size_t seq1_size = seq1.size();
    std::size_t seq2_size = seq2.size();

    /** INITIALISATION **/
    //Computing Pixel mean of each image
    std::vector<double> means1, means2;
    means1.reserve(seq1_size);
    means2.reserve(seq2_size);
    for(const cv::Mat& image : seq1)
        means1.push_back(cv::mean(cv::mean(image))[0]);

    for(const cv::Mat& image : seq2)
        means2.push_back(cv::mean(cv::mean(image))[0]);

    /** COMPUTATION **/
    //We try to found the longest subsequence of zeros for each offset
    const std::size_t gliding_length = 2*seq1_size;

    std::vector<std::size_t> max_zeros, max_zeros_ind1, max_zeros_ind2;
    max_zeros.reserve(gliding_length-1);
    max_zeros_ind1.reserve(gliding_length-1);
    max_zeros_ind2.reserve(gliding_length-1);

    std::vector< std::vector<int> > result_seqs;
    result_seqs.reserve(gliding_length-1);

    std::ofstream file_zeros("/home/bdrapeaud/Bureau/zero.txt");
    for(std::size_t o=1; o<gliding_length; o++)
    {
        /** Size of the sequence to analyse **/
        std::size_t analyse_length;
        /** Vector of difference of the sequence means **/
        std::vector<int> mean_diffs;

        if(o < seq1_size)
        {
            analyse_length = o;
            mean_diffs.reserve(analyse_length);
            for(std::size_t i=0; i<analyse_length; i++)
            {
                mean_diffs.push_back(
                    static_cast<int>(
                        std::abs(means1[i]-means2[seq1_size-analyse_length+i])));
            }
        }
        else
        {
            analyse_length = gliding_length-o;
            mean_diffs.reserve(analyse_length);
            for(std::size_t i=0; i<analyse_length; i++)
            {
                mean_diffs.push_back(
                    static_cast<int>(
                        std::abs(means1[seq1_size-analyse_length+i]-means2[i])));
            }
        }

        //Smoothing the result to cancel noise
//        utils::exponential_smoothing(&mean_diffs, 0.5);
        utils::denoise(&mean_diffs,25,0);

        //Find the longest zeros sequence
        std::size_t zero_seq = 0, max_zero_seq = 0;
        std::size_t zeros_ind1 = 0, zeros_ind2 = 0;

        for(std::size_t i=0; i<analyse_length; i++)
        {
            if(utils::more_less(mean_diffs[i],0,0))
                zero_seq++;
            else
            {
                if(zero_seq > max_zero_seq)
                {
                    max_zero_seq = zero_seq;
                    if(o > seq1_size)
                    {
                        zeros_ind1 = i-zero_seq + (o-seq1_size);
                        zeros_ind2 = zeros_ind1 - (o-seq1_size);
                    }
                    else
                    {
                        zeros_ind1 = i-zero_seq + (seq1_size-o);
                        zeros_ind2 = zeros_ind1 + (o-seq1_size);
                    }
                }
                zero_seq = 0;
            }
        }

        //Adding longest zero sequence to log file
        file_zeros << max_zero_seq << std::endl;

        //Add result to main vectors
        max_zeros.push_back(max_zero_seq);
        max_zeros_ind1.push_back(zeros_ind1);
        max_zeros_ind2.push_back(zeros_ind2);
        result_seqs.push_back(mean_diffs);

        std::cout << "2) SEARCHING: "
                  << std::fixed << std::setprecision(1)
                  << (100.0f/static_cast<float>(gliding_length))
                  *static_cast<float>(o) <<'%'<< "\e[A"
                  << std::endl;
    }
    std::cout << std::endl;

    file_zeros.close();


    //We try to found wich sequence of zeros was the longest
    for(std::size_t i=0; i<gliding_length-1; i++)
    {
        if(max_zeros[i]>*sequence_length)
        {
            *sequence_length = max_zeros[i];

            if(i > seq1_size)
            {
                *sequence1_begin = max_zeros_ind1[i];
                *sequence2_begin = max_zeros_ind2[i];
            }

            else
            {
                *sequence1_begin = max_zeros_ind2[i];
                *sequence2_begin = max_zeros_ind1[i];
            }

        }

        std::cout << "3) FINALIZING: "
                  << std::fixed << std::setprecision(1)
                  << (100.0f/static_cast<float>(gliding_length))
                  *static_cast<float>(i) <<'%' << "\e[A"
                  << std::endl;
    }
    std::cout << std::endl;

    //We write the successful sequence to a file*
    if(*sequence1_begin != std::size_t(-1))
    {
        std::ofstream file("/home/bdrapeaud/Bureau/test.txt");
        for(int u : result_seqs[*sequence1_begin])
            file << u << std::endl;
        file.close();

        return true;
    }

    return false;
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

    //Determining number of zeros of the sequence
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

    // Getting all the paths
    for(; dir_it != end_it; dir_it.increment(error_code))
    {
        if(error_code)
            throw exception("Boost error occured",
                            "load_image_from_dir",
                            error_code.message());

        std::string file_name = dir_it->path().stem().string();
        paths.push_back(dir_it->path());

        // We extract the number of the image's path
        image_nums.push_back(
            std::stoi(
                file_name.substr(
                    file_name.size()-number_zeros,number_zeros)));
    }

    // Sort the path as sequence
    std::size_t seq_size = image_nums.size();
    std::vector<boost::filesystem::path> sorted_paths(seq_size);
    for(std::size_t i=0; i<seq_size; i++)
    {
        std::size_t ind =
            static_cast<std::size_t>(image_nums[i])-1;
        sorted_paths[ind] = paths[i];
    }

    // Load image
    float progression = 0.0f;
    images.reserve(seq_size);
    for(const boost::filesystem::path& path: sorted_paths)
    {
        cv::Mat image = cv::imread(path.string(), CV_LOAD_IMAGE_GRAYSCALE);
        if(!image.empty())
            images.emplace_back(image);

        std::cout << "1) LOADING: " << path.filename()  << "\e[A" << std::endl;
        progression++;
    }
    std::cout << std::endl;

    return images;
}
}
