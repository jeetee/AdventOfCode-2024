#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <algorithm>
#include <array>
#include <execution>
#include <list>
#include <vector>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using std::cout;
using std::endl;
using std::list;
using std::string;
using std::string_literals::operator""s;
using std::string_view;
using std::string_view_literals::operator""sv;
using std::vector;
using std::unordered_map;
using std::unordered_set;

static void logic(string fileName);

int main()
{
    cout << "Advent of Code 2024: 11\n";
    logic("11-example.txt"s);
    cout << endl;
    logic("11-input.txt"s);
    return 0;
}

static unsigned int num_digits(unsigned long long number);

static void logic(string fileName)
{
    list<unsigned long long> stones;

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    
    for (string line; std::getline(input, line);) {
        std::stringstream ss(line);
        for (unsigned long long stone; (ss >> stone);) {
            stones.emplace_back(stone);
        }
    }

    // PART 1
    for (int blink = 25; blink --> 0;) {
        cout << '.' << std::flush; // Progress report
        for (auto stone = std::begin(stones); stone != std::end(stones); ++stone) {
            auto digits = num_digits(*stone);
            if (*stone == 0) {
                *stone = 1;
            }
            else if ((digits % 2) == 0) {
                digits = pow(10, digits / 2);
                stones.emplace(stone, *stone / digits);
                *stone %= digits;
            }
            else {
                *stone *= 2024;
            }
        }
    }

    cout << "\nNumber of stones after 25 blinks = " << stones.size() << endl;
}

static unsigned int num_digits(unsigned long long number)
{
    unsigned int num_digits = 0;
    do {
        ++num_digits;
    } while (number /= 10);
    return num_digits;
}
