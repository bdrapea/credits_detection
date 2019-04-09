#include "crde_utils.h"

namespace crde
{
    namespace  utils
    {
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
