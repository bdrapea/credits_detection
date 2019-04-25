#include "crde_utils.h"

namespace crde
{
namespace  utils
{
std::string frame_to_time(std::size_t frames, const float fps)
{
    std::size_t nb_frames = frames % static_cast<std::size_t>(fps);
    frames /= static_cast<std::size_t>(fps);
    std::size_t nb_seconds = frames % 60;
    frames /= 60;
    std::size_t nb_minutes = frames % 60;
    frames /= 60;
    std::size_t nb_hours = frames % 60;
    std::stringstream times;

    auto two_number_display = [](const std::size_t num)->std::string
    {
        std::string num_str;
        if(num < 10)
            num_str = std::string("0") + std::to_string(num);
        else
            num_str = std::to_string(num);

        return num_str;
    };

    times << two_number_display(nb_hours);
    times << ':';
    times << two_number_display(nb_minutes);
    times << ':';
    times << two_number_display(nb_seconds);
    times << '.';
    times << two_number_display(nb_frames);

    return times.str();
}

std::ostream& operator<<(std::ostream& os,
                         const utils::credits_tc& timecodes)
{
    const std::size_t count = timecodes.starts.size();

    os << "=========TIMECODES=========\n";

    for(std::size_t i=0; i<count; i++)
    {
        os << std::setw(6)
           << "Video: " << timecodes.video_names[i] << '\n'
           << "Times:    "
           << frame_to_time(timecodes.starts[i],25.0f)
           << " --> "
           << frame_to_time(timecodes.ends[i],25.0f) << '\n'
           << "Frames:   "
           << timecodes.starts[i]
           << " --> "
           << timecodes.ends[i] << '\n'
           << "Duration: "
           << frame_to_time(timecodes.ends[i]-timecodes.starts[i],25.0f)
           << '\n'
           << "\n\n";

    }

    return os;
}
}
}
