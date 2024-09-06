#include <alg/alg.hpp>

#include <fstream>
#include <ios>
#include <rang.hpp>
#include <stdexcept>
using namespace rang;

#include <iomanip>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <ratio>
#include <string>
#include <random>
#include <chrono>
#include <cmath>

#include <cxxopts.hpp>

struct bench_result
{
    size_t inputSize;
    size_t interCount;
    double averageUs;
    double deviationUs;
};

std::ostream& operator<<(std::ostream& os, const bench_result& result)
{
    return os << std::fixed
        << "input-size=" << result.inputSize << " "
        << result.averageUs << " us +- " << result.deviationUs << " us ("
        << result.interCount << " iterations)";
}

std::ostream& errout()
{
    return std::cerr << style::bold << fg::red << "error: " << style::reset;
}

// average_deviation returns average sample deviation from sample average.
template <class T>
double average_deviation(T& window)
{
    if (window.size() == 0)
    {
        exit(1);
    }
    
    if (window.size() == 1)
    {
        return 0;
    }
    
    double sum{};
    for (auto const& e : window)
        sum += e;

    double avg = sum / window.size();

    double deltaSum{};
    for (auto const& e : window)
    {
        deltaSum += std::abs(e - avg);
    }
    
    return deltaSum / window.size();
};

template <class T>
double average(T const& v)
{
    double sum = 0;
    for (auto& e : v)
    {
        sum += e;
    }

    return sum / std::size(v);
}

template <class F>
bench_result average_time(F&& fn, size_t inputSize, size_t minCount = 100, double targetDevUs = 0.1)
{
    double curSum = 0.0;
    double answerSum = 0.0;
    size_t iterCount = 0;
    
    std::vector<double> avgWindow;
    avgWindow.reserve(minCount);
    
    std::vector<double> deltaWindow;
    avgWindow.reserve(minCount);
    
    std::mt19937 rd(time(0));
    std::uniform_int_distribution<int> dist(0, 1);
    while (iterCount < minCount || average_deviation(avgWindow) > targetDevUs)
    {
        std::string input(inputSize, '\0');
        for (auto& ch : input)
        {
            ch = dist(rd) == 0 ? '_' : '#';
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();

        std::forward<F>(fn)(input);

        auto elapsed = std::chrono::high_resolution_clock::now() - startTime;
        auto elapsedUs = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1'000'000>>>(elapsed).count();
        
        iterCount++;
        curSum += elapsedUs;
        if (iterCount > minCount)
        {
            avgWindow.erase(avgWindow.begin());
            deltaWindow.erase(deltaWindow.begin());
        }

        avgWindow.push_back(curSum / iterCount);
        deltaWindow.push_back(average_deviation(avgWindow));
        
        if (iterCount % 50000 == 0)
        {
            std::cerr << "iter " << iterCount / 1000 << "k: avg " << avgWindow.back() << "us +- " << average_deviation(avgWindow) << "us\n";
        }
    }
    
    return {inputSize, iterCount, avgWindow.back(), average_deviation(avgWindow)};
}

int main(int argc, char* argv[])
{
    cxxopts::Options options("Island bridge solutions benchmark",
            "Benchmarks algorithms on random strings for a certain size range and produces a CSV report");

    options.add_options()
        ("window-size", "size of the delta window (dev)", cxxopts::value<size_t>()->default_value("1000"))
        ("dev-us", "target deviation of the sample set in microseconds", cxxopts::value<double>()->default_value("0.001"))
        ("from", "starting input string length", cxxopts::value<size_t>()->default_value("1"))
        ("to", "last input string length", cxxopts::value<size_t>()->default_value("1"))
        ("csv", "print algorithm stats as CSV")
        ("out", "duplicate stdout to this file, won't overwrite", cxxopts::value<std::string>()->default_value(""));

    auto args = options.parse(argc, argv);

    if (args.count("from") == 0 || args.count("to") == 0)
    {
        std::cerr << options.help() << '\n';
        errout() << "--from and --to are required" << std::endl;
        return 1;
    }
    
    std::ostream* out = &std::cout;
    
    std::ofstream fileOut;
    if (args.count("out"))
    {
        auto filename = args["out"].as<std::string>();
        
        std::error_code ec;
        if (std::filesystem::exists(filename, ec))
        {
            errout() << "output file " << filename << " already exists\n";
            return 1;
        }
        else if (ec)
        {
            errout() << "check existance of " << filename << ": os error " << ec.value() << " (" << ec.message() << ")\n";
            return 1;
        }
        
        try
        {
            fileOut.open(filename);
        }
        catch (std::runtime_error& err)
        {
            errout() << "open " << filename << "for writing: " << err.what() << '\n';
            return 1;
        }
        out = &fileOut;
    }
    
    if (args.count("csv"))
    {
        *out << "N,No fancy, No fancy dev us,Range-v3, Range-v3 dev us,Ratio\n";
    }
    
    auto from = args["from"].as<size_t>();
    auto to = args["to"].as<size_t>();
    
    auto lastProgress = std::chrono::steady_clock::now();
    auto startProgress = lastProgress;
    for (size_t inputSize = from; inputSize <= to; inputSize++)
    {
        auto now = std::chrono::steady_clock::now();
        if (now - lastProgress > std::chrono::seconds(1))
        {
            auto elapsed = now - startProgress;
            auto elapsedMin = std::chrono::duration_cast<std::chrono::minutes>(elapsed).count();
            auto elapsedSec = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() % 60;
            
            std::cerr << ' ' << style::dim;
            
            if (elapsedMin != 0)
                std::cerr << elapsedMin << "m";
            
            std::cerr << elapsedSec << "s " << style::reset
                << "Progress: " << 100 * (inputSize - from) / (to - from + 1) << "% ("
                << inputSize << "/" << to << ")     \r";
            lastProgress = now;
        }

        if (args["csv"].count())
        {
            *out << inputSize << ',';
        }

        auto benchNoFancy = average_time(alg::no_fancy, inputSize, args["window-size"].as<size_t>(), args["dev-us"].as<double>());
        if (args["csv"].count())
        {
            *out << std::fixed << benchNoFancy.averageUs << ',' << benchNoFancy.deviationUs << ',';
        }
        else
        {
            *out << "no fancy: " << benchNoFancy << '\n';
        }

        auto benchRanges = average_time(alg::with_ranges, inputSize, args["window-size"].as<size_t>(), args["dev-us"].as<double>());
        if (args["csv"].count())
        {
            *out << std::fixed
            << benchRanges.averageUs << ',' << benchRanges.deviationUs << ','
            << benchRanges.averageUs / benchNoFancy.averageUs << '\n';
        }
        else
        {
            *out << "with ranges: " << benchRanges << '\n';
        }
    }
    
    std::cerr << '\n';

    return 0;
}
