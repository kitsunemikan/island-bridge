#include <alg/alg.hpp>

#include <boost/ut.hpp>
using namespace boost::ut;

#include <string>
#include <vector>
#include <concepts>

struct TestCase
{
    std::string input;
    int want;
};

std::vector<TestCase> testCases{
    {"", 0},
    {"_", 0},
    {"#", 0},
    {"#_", 0},
    {"_#", 0},
    {"##", 0},
    {"__", 0},
    {"#__#", 0},
    {"#_#", 2},
    {"#_#_#", 2},
    {"#_##__#", 3},
    {"_##__#", 0},
    {"#__##_", 0},
    {"_##_", 0},
    {"#_##_###_##_#", 5},
    {"#_#__#__", 2},
    {"_#_##", 3},
    {"#__#__#__", 0},
    {"#__#_#_#", 2},
    {"_##__###", 0},
};

static size_t bytesAllocated = 0;

void* operator new(size_t size)
{
    bytesAllocated += size;
    return malloc(size);
}

void operator delete(void* ptr) noexcept
{
    free(ptr);
}

void run_suite(std::invocable<std::string_view> auto alg)
{
    std::vector<size_t> memoryAllocated(testCases.size());

    for (int i = 0; i < testCases.size(); i++)
    {
        auto const& test = testCases[i];

        should(test.input) = [&] {
            auto bytesAllocatedBefore = bytesAllocated;

            auto got = alg(test.input);

            memoryAllocated[i] = bytesAllocated - bytesAllocatedBefore;

            expect(got == test.want) << "for " << test.input << "got" << got << "but want" << test.want;
        };
    }

    "constant_memory"_test = [&] {
        for (int i = 1; i < memoryAllocated.size(); i++)
        {
            if (memoryAllocated[i] != memoryAllocated[0])
            {
                std::stringstream ss;

                for (auto bytes : memoryAllocated)
                {
                    ss << bytes << ' ';
                }

                expect(false >> fatal) << "algorithm is not constant in memory.\n"
                    "  By test (bytes): " << ss.str();
                break;
            }
        }
    };
}

int main()
{
    "no fancy"_test = [] {
        run_suite(alg::no_fancy);
    };

    "range-v3"_test = [] {
        run_suite(alg::with_ranges);
    };

    return 0;
}
