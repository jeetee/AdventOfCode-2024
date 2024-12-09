#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <algorithm>
#include <execution>
#include <vector>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using std::cout;
using std::endl;
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
    cout << "Advent of Code 2024: 09\n";
    logic("09-example.txt"s);
    cout << endl;
    logic("09-input.txt"s);
    return 0;
}

static void logic(string fileName)
{
    vector<int> disk_blocks; // Block value [0+ == file_ID, <0 == gap

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    unsigned int file_ID{ 0 };
    for (string line; std::getline(input, line);) {
        bool gap = false;
        // Expand
        for (const char& c : line) {
            for (size_t size = static_cast<size_t>(c - '0'); size --> 0;) {
                disk_blocks.emplace_back(gap? -1 : file_ID);
            }
            if (!gap) {
                ++file_ID;
            }
            gap = !gap;
        }
    }

    // PART 1
    cout << "Fragmenting ";
    for (size_t left = 0, right = disk_blocks.size() - 1; left < right; ++left) {
        if (disk_blocks[left] < 0)
        {
            cout << '.' << std::flush; // Progress report
            while (disk_blocks[right] < 0)
            { // Skip gaps on the end
                --right;
            }
            // Swap & erase
            disk_blocks[left] = disk_blocks[right];
            disk_blocks[right] = -1;
        }
    }

    unsigned long checksum{ 0 };
    for (size_t idx = 0; (idx < disk_blocks.size()) && (disk_blocks[idx] >= 0); ++idx) {
        checksum += idx * disk_blocks[idx];
    }

    cout << "\nFragmented disk checksum = " << checksum << endl;
}
