#include <alg/alg.hpp>

#include <range/v3/all.hpp>

#include <functional>
#include <array>

namespace views = ranges::views;

namespace alg
{

size_t with_ranges(std::string_view input)
{
    if (input.empty())
        return 0;

    auto sliding = input
        | views::group_by(std::equal_to<>{})
        | views::transform([](auto sgmt) {
            return std::make_pair(sgmt[0], ranges::size(sgmt));
        })
        | views::sliding(3);

    size_t unionMax = 0;
    for (const auto& windowView : sliding)
    {
        auto window = std::array<std::pair<char, size_t>, 3>();
        ranges::copy(windowView, window.begin());
        
        if (window[0].first != '#' || window[1].second != 1)
            continue;

        size_t cur = window[0].second + window[2].second;
        unionMax = std::max(unionMax, cur);
    }

    return unionMax;
}

} // namespace alg