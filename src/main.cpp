#include "credits_detection.h"

int main(int argc, char** argv)
{
    /** Loads the inputs from the console in a path vector **/
    std::vector< boost::filesystem::path > biff_directories;
        biff_directories.reserve(
        static_cast<std::size_t> (argc-1));

    if(argc <= 2)
    {
        std::cerr << "Please provide at least two directories" << std::endl;
        return 0;
    }

    for(int i=1; i<argc; i++)
        biff_directories.emplace_back(argv[i]);

    /** Main process **/
    crde::utils::credits_tc timecodes;
    try
    {
        timecodes = crde::find_credits_timecodes(biff_directories);
    }
    catch(const crde::exception& except)
    {
        std::cout << except << std::endl;
        return -1;
    }

    /** Prints the timecodes **/
    std::cout << timecodes << std::endl;

    return 0;
}
