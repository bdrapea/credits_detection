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
    // Initialize return value
    utils::credits_tc timecodes;
    timecodes.video_names = video_names;
    timecodes.starts.assign(path_count,0);
    timecodes.ends.assign(path_count,0);

    /**< Flag telling if we found the credits while comparing the
     * two sequences **/
    bool credits_found;
    /**< Length of credits in frames **/
    std::size_t credits_length = 0;

    //MAIN ALGORITHM
    //Find the repeating pattern
    credits_length = 0;
    credits_found = find_longest_common_sequence(
                        sequences[0],
                        sequences[1],
                        &timecodes.starts[0],
                        &timecodes.starts[1],
                        &credits_length);
    timecodes.ends[0] = timecodes.starts[0] + credits_length;

    if(!credits_found)
    {
        std::cerr << "Can't find credits" << std::endl;
        return utils::credits_tc();
    }

    if(path_count == 2)
        return timecodes;

    credits_found = find_longest_common_sequence(
                        sequences[1],
                        sequences[2],
                        &timecodes.starts[1],
                        &timecodes.starts[2],
                        &credits_length);
    timecodes.ends[1] = timecodes.starts[1] + credits_length;

    if(!credits_found)
    {
        std::cerr << "Can't find credits" << std::endl;
        return utils::credits_tc();
    }

    //Getting the two repeating sequence from checking
    std::vector< cv::Mat > sub_seq1 =
        utils::sub_vector(sequences[0],
                          timecodes.starts[0],
                          timecodes.ends[0] - timecodes.starts[0]);
    std::vector< cv::Mat > sub_seq2 =
        utils::sub_vector(sequences[1],
                          timecodes.starts[1],
                          timecodes.ends[1] - timecodes.starts[1]);

    std::size_t sub_seq1_size = sub_seq1.size();
    std::size_t sub_seq2_size = sub_seq2.size();

    std::cout << "CHECKING COMMON SEQUENCES: " << std::flush;

    if(sub_seq1_size > sub_seq2_size)
        credits_found = search_for_subsequence(sub_seq2,sub_seq1,0.8f);
    else
        credits_found = search_for_subsequence(sub_seq1,sub_seq2,0.8f);

    if(!credits_found)
    {
        std::cerr << "Common sub_sequence are different: Can't find credits"
                  << std::endl;
        return utils::credits_tc();
    }

    std::cout << "OK" << std::endl;

    //Searching common sequence in other episode
    std::vector< cv::Mat > sequence_to_find =
        utils::sub_vector(sequences[0], timecodes.starts[0], credits_length);
    float ressemblance = 0.99f;

    for(std::size_t i= 0; i<path_count-1; i++)
    {
        bool found = search_for_subsequence(
                         sequence_to_find,
                         sequences[i],
                         ressemblance,
                         &timecodes.starts[i],
                         &credits_length);


        timecodes.ends[i] = timecodes.starts[i] + credits_length;
        if(!found)
        {

            while(!found)
            {
                ressemblance -= 0.01f;
                std::cout << "i=" << i <<' ' << ressemblance <<  std::endl;
                found = search_for_subsequence(
                                 sequence_to_find,
                                 sequences[i],
                                 ressemblance,
                                 &timecodes.starts[i],
                                 &credits_length);

                if(ressemblance < 0.5f)
                {
                    std::cerr << "Can't find timecode" << std::endl;
                    return utils::credits_tc();
                }


            }

            sequence_to_find = utils::sub_vector(sequences[i+1],
                                                 timecodes.starts[i+1],
                                                 credits_length);

            ressemblance = 0.99f;
        }

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

    std::ofstream file1("/home/vqserver2/Bureau/t1.txt"), file2("/home/vqserver2/Bureau/t2.txt");
    for(const cv::Mat& image : seq1)
    {
        means1.push_back(cv::mean(cv::mean(image))[0]);
        file1 << cv::mean(cv::mean(image))[0] << '\n';
    }

    for(const cv::Mat& image : seq2)
    {
        means2.push_back(cv::mean(cv::mean(image))[0]);
        file2 <<cv::mean(cv::mean(image))[0] << '\n';
    }

    file1.close();
    file2.close();

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
            mean_diffs.assign(analyse_length,0);
            for(std::size_t i=0; i<analyse_length; i++)
            {
                mean_diffs[i]=
                    static_cast<int>(
                        std::abs(means1[i]-means2[seq1_size-analyse_length+i]));
            }
        }
        else
        {
            analyse_length = gliding_length-o;
            mean_diffs.assign(analyse_length,0);
            for(std::size_t i=0; i<analyse_length; i++)
            {
                mean_diffs[i]=
                    static_cast<int>(
                        std::abs(means1[seq1_size-analyse_length+i]-means2[i]));
            }
        }

        //Smoothing the result to cancel noise
//        utils::denoise(&mean_diffs,10,0);

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

        std::cout << "SEARCH COMMON SEQUENCE: "
                  << std::fixed << std::setprecision(1)
                  << (100.0f/static_cast<float>(gliding_length))
                  *static_cast<float>(o) <<'%'<< "\e[A" << std::endl;
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
    }

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

bool search_for_subsequence(
    const std::vector< cv::Mat >& subsequence,
    const std::vector< cv::Mat >& sequence,
    const float tolerance,
    std::size_t* start,
    std::size_t* length)
{
    std::size_t sub_size = subsequence.size();
    std::size_t seq_size = sequence.size();

    //Compute mean of picel for each sequence
    std::vector<int> means_sub, means;
    means_sub.reserve(sub_size);
    means.reserve(seq_size);
    for(const cv::Mat& image : subsequence)
        means_sub.push_back(static_cast<int>(
                                cv::mean(cv::mean(image))[0]));

    for(const cv::Mat& image : sequence)
        means.push_back(static_cast<int>(
                            cv::mean(cv::mean(image))[0]));

    //We try to find the sub sequence in it
    std::size_t max = 0, ind = 0;
    std::size_t gliding_length = seq_size - sub_size;

    for(std::size_t i=0; i<gliding_length+1; i++)
    {
        std::size_t count = 0;
        for(std::size_t j=sub_size-1; j>0; j--)
        {
            if(utils::more_less(means[i+j],means_sub[j],1))
                count++;
        }

        if(count > max)
        {
            max = count;
            ind = i;
        }
    }

    std::cout << ind << ' ' << max << std::endl;

    const std::size_t minimum_length = static_cast<std::size_t>(
                                           static_cast<float>(sub_size)*tolerance);

    if(start != nullptr)
        *start = ind;
    if(length != nullptr)
        *length = max;

    if(max >= minimum_length)
        return true;

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

        std::cout << "LOADING BIFF: " << path.filename()  << "\e[A" << std::endl;
        progression++;
    }
    std::cout << std::endl;

    return images;
}
}
