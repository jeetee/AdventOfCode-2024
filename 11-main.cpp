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
                digits = static_cast<unsigned int>(pow(10, digits / 2));
                stones.emplace(stone, *stone / digits);
                *stone %= digits;
            }
            else {
                *stone *= 2024;
            }
        }
    }
    cout << "\nNumber of stones after 25 blinks = " << stones.size() << endl;

    // PART 2 - for a total of 75 blinks we'd run out of memory to brute force this.
    // So we'd need a mathematical approach instead:
    // - stones don't influence each other, so order is actually irrelevant
    // - the number on a stone completely defines all future generations of it
    // So we could track how many stones there are for each number and then run our logic on that instead
    unordered_map<unsigned long long /*number on stone*/, unsigned long long /*count*/> stone_map;
    // Build on top of PART 1's 25 iterations
    for (auto& stone : stones) {
        stone_map[stone]++;
    }
    cout << "Next 50 blinks using frequency map " << endl;
    for (int blink = 50; blink --> 0;) {
        cout << '.' << std::flush; // Progress report
        unordered_map<unsigned long long /*number on stone*/, unsigned long long /*count*/> next_stone_map;
        for (const auto& [stone, count] : stone_map) {
            auto digits = num_digits(stone);
            if (stone == 0) {
                next_stone_map[1] += count;
            }
            else if((digits % 2) == 0) {
                digits = static_cast<unsigned int>(pow(10, digits / 2));
                next_stone_map[stone / digits] += count;
                next_stone_map[stone % digits] += count;
            }
            else {
                next_stone_map[stone * 2024ull] += count;
            }
        }
        stone_map = next_stone_map;
    }
    cout << "\nNumber of stones after 75 blinks = " << std::accumulate(std::begin(stone_map), std::end(stone_map), 0ull, [](unsigned long long sum, const auto& stone_info) {
            return sum + stone_info.second;
        }) << endl;
}

static unsigned int num_digits(unsigned long long number)
{
    unsigned int num_digits = 0;
    do {
        ++num_digits;
    } while (number /= 10);
    return num_digits;
}
