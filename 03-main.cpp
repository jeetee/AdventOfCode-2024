#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <regex>

using std::cout;
using std::endl;
using std::string;
using std::string_literals::operator""s;

static void logic(string fileName);

int main()
{
    cout << "Advent of Code 2024: 03\n";
    logic("03-example.txt"s);
    cout << endl;
    logic("03-example-2.txt"s);
    cout << endl;
    logic("03-input.txt"s);
    return 0;
}

static void logic(string fileName)
{
    cout << "Reading " << fileName << " file " << std::flush;
    std::string fileContents;
    {
        std::ifstream input(fileName);
        std::stringstream ss;
        ss << input.rdbuf();
        fileContents = ss.str();
    }
    cout << "." << std::flush; // File reading done

    const std::regex mul("mul\\(([1-9][0-9]{0,2}),([1-9][0-9]{0,2})\\)|do\\(\\)|don't\\(\\)"s);
    const auto mul_begin = std::sregex_iterator(fileContents.begin(), fileContents.end(), mul);
    const auto mul_end = std::sregex_iterator();

    unsigned long sum = 0, sum_enabled = 0;
    bool enabled = true;
    for (auto it = mul_begin; it != mul_end; ++it)
    {
        const auto match = *it;
        // 2nd char is unique in all three options, so use it to identify
        switch (match.str()[2])
        {
            case 'l': // mul()
            {
                // No index safety checking as the fact that we matched the regex should guarantee success here
                const unsigned long multiplication = std::stoul(match[1].str()) * std::stoul(match[2].str());
                sum += multiplication;
                if (enabled)
                {
                    sum_enabled += multiplication;
                }
            } break;

            case '(': // do()
            {
                enabled = true;
            } break;

            case 'n': // don't()
            {
                enabled = false;
            }
        }
    }

    cout << "\nSum of multiplications = " << sum
         << "\nSum of enabled multiplications = " << sum_enabled << endl;
}
