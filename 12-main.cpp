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
    cout << "Advent of Code 2024: 12\n";
    logic("12-example.txt"s);
    cout << endl;
    logic("12-example-2.txt"s);
    cout << endl;
    logic("12-example-3.txt"s);
    cout << endl;
    logic("12-input.txt"s);
    return 0;
}

struct SizeTPairHasher {
    std::size_t operator()(const std::pair<size_t, size_t>& val) const {
        std::stringstream ss;
        ss << val.first << '|' << val.second;
        return std::hash<string>{}(ss.str());
    }
};

static void logic(string fileName)
{
    vector<string> lines;

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    
    for (string line; std::getline(input, line);) {
        cout << '.' << std::flush; // Progress report
        lines.emplace_back(line);
    }

    // PART 1
    unsigned long long total_fence_price{ 0 };
    const size_t height = lines.size();
    const size_t width = lines.front().size();
    for (size_t y = height; y --> 0;) {
        for (size_t x = width; x --> 0;) {
            if ((lines[y][x] < 'A') || (lines[y][x] > 'Z')) {
                continue;
            }
            // New area found
            const char current_region = lines[y][x];
            const char current_region_processed = current_region - 'A' + 'a';
            unsigned int area{ 0 }, perimeter{ 0 };
            unordered_set<std::pair<size_t, size_t>, SizeTPairHasher> area_points_to_inspect{ std::make_pair(x, y) };
            do {
                const auto& current_point = *(area_points_to_inspect.begin());
                ++area;
                lines[current_point.second][current_point.first] = current_region_processed; // Prevent double processing of this position

                // Check fence and/or area expansion
                const vector<std::pair<size_t, size_t>> neighbors{ { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };
                for (const auto& [xOffset, yOffset] : neighbors) {
                    const auto next_pos_x = current_point.first + xOffset;
                    const auto next_pos_y = current_point.second + yOffset;

                    if ((next_pos_x < 0) || (next_pos_x >= width) || (next_pos_y < 0) || (next_pos_y >= height)
                        || ((lines[next_pos_y][next_pos_x] != current_region) && (lines[next_pos_y][next_pos_x] != current_region_processed))
                        ) { // Out of bounds or not the same region --> fence
                        ++perimeter;
                    }
                    else {
                        // Same region
                        if (lines[next_pos_y][next_pos_x] == current_region) {
                            area_points_to_inspect.emplace(std::make_pair(next_pos_x, next_pos_y));
                        }
                    }
                }

                area_points_to_inspect.erase(current_point);
            } while (!area_points_to_inspect.empty());

            total_fence_price += static_cast<unsigned long long>(area) * perimeter;
        }
    }

    cout << "\nTotal fence price = " << total_fence_price << endl;
}
