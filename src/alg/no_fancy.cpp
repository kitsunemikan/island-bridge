#include <alg/alg.hpp>

#include <array>
#include <algorithm>

namespace alg
{

size_t no_fancy(std::string_view input)
{
    int unionMax = 0;
    
    int pendingStart = 0;
    int segmentsFound = 0;
    std::array<std::pair<char, int>, 3> window{};
    for (int i = 1; i < input.size() + 1; i++)
    {
        if (i < input.size() && input[i] == input[pendingStart])
            continue;

        window[0] = window[1];
        window[1] = window[2];
        window[2] = {input[pendingStart], i - pendingStart};
        pendingStart = i;
        segmentsFound++;

        if (segmentsFound < 3)
            continue;

        if (window[0].first != '#' || window[1].second != 1)
            continue;

        unionMax = std::max(unionMax, window[0].second + window[2].second);
    }
    
    return unionMax;
}

} // namespace alg
