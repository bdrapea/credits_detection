#include "crde_utils.h"

namespace crde
{
namespace  utils
{
std::ostream& operator<<(std::ostream& os,
                         const utils::credits_tc& timecodes)
{
    std::size_t count = timecodes.starts.size();

    os << "=========TIMECODES=========\n";

    for(std::size_t i=0; i<count; i++)
    {
        os << "Video: " << timecodes.video_names[i] << '\n'
           << timecodes.starts[i] << " --> " << timecodes.ends[i] << "\n\n";
    }

    return os;
}
}
}
