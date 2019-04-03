#include "credits_detection.h"

namespace crde
{
    credits_tc find_credits_timecodes(
            const std::vector< boost::filesystem::path >& biff_folders)
    {
        /** Basic checks **/
        std::vector< boost::filesystem::path >::size_type path_count = biff_folders.size();
        for(boost::filesystem::path path : biff_folders)
        {
            /** Check if the path lead to a directory **/
            if(!boost::filesystem::is_directory(path))
                throw exception("Path not valid:",
                                "find_credits_timecodes",
                                "Path doesn't lead to a directory:"+ path.string());

            /** Check if the path exist **/
            if(!boost::filesystem::exists(path))
                throw exception("Path not valid:",
                                "find_credits_timecodes",
                                "Path doesn't exist" + path.string());
        }

        credits_tc timecodes;
            timecodes.starts.assign(path_count,0);
            timecodes.ends.assign(path_count,0);
            timecodes.video_names.assign(path_count,"");

        return timecodes;
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
}
