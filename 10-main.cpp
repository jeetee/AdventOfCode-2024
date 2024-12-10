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
    cout << "Advent of Code 2024: 10\n";
    logic("10-example.txt"s);
    cout << endl;
    logic("10-input.txt"s);
    return 0;
}

typedef struct Point_s
{
    int x;
    int y;

    bool operator==(const struct Point_s& p) const
    {
        return (x == p.x) && (y == p.y);
    }
    
    struct Point_s operator+(const struct Point_s& p) const
    {
        return Point_s{ x + p.x, y + p.y };
    }
} Point_t;
struct PointClassHash
{
    std::size_t operator()(const Point_t& p) const
    {
        std::stringstream ss;
        ss << p.x << '|' << p.y;
        return std::hash<string>{}(ss.str());
    }
};

typedef struct Trail_Head_s
{
    Point_t start;
    std::unordered_set<Point_t, PointClassHash> endings;
    unsigned int rating{ 0 };
} Trail_Head_t;

static void logic(string fileName)
{
    vector<string> lines;
    vector<Trail_Head_t> trailheads;
    const char PATH_START = '0';
    const char PATH_END = '9';

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    int x{ 0 }, y{ 0 };
    for (string line; std::getline(input, line); ++y) {
        cout << '.' << std::flush; // Progress report
        lines.emplace_back(line);

        // Find possible tail heads
        for (x = static_cast<int>(line.size()); x --> 0;) {
            if (line[x] == PATH_START) {
                trailheads.emplace_back(Trail_Head_t{ Point_t{ x, y } });
            }
        }
    }

    // PART 1 + 2
    cout << "\nAnalysing " << trailheads.size() << " trail heads ";
    const int height{ static_cast<int>(lines.size()) };
    const int width{ static_cast<int>(lines.front().size()) };
    for (auto& trailhead : trailheads) {
        // BFS
        cout << '.' << std::flush; // Progress report
        vector<Point_t> path_pos{ trailhead.start };
        do {
            vector<Point_t> next_positions;
            for (auto& current_pos : path_pos) {
                // Trace possible next positions for this path_pos entry
                const char next_level = lines[current_pos.y][current_pos.x] + 1;
                const std::array<Point_t, 4> moves{ Point_t{ -1, 0 }, Point_t{ 0, -1 }, Point_t{ 1, 0 }, Point_t{ 0, 1 } };
                for (auto& move : moves) {
                    Point_t newPos = current_pos + move;
                    if ((newPos.x >= 0) && (newPos.x < width) && (newPos.y >= 0) && (newPos.y < height)
                        && (lines[newPos.y][newPos.x] == next_level)
                        )
                    {// This is a path continuation
                        if (next_level == PATH_END) {
                            trailhead.endings.emplace(newPos);
                            trailhead.rating++;
                        }
                        else {
                            // Continue looking down this path
                            next_positions.emplace_back(newPos);
                        }
                    }
                }
            }
            path_pos = next_positions;
        } while (!path_pos.empty());
    }

    const size_t sum_of_trail_scores = std::accumulate(std::cbegin(trailheads), std::cend(trailheads), 0ull, [](size_t sum, const auto& trailhead) {
        return sum + trailhead.endings.size();
    });
    const size_t sum_of_trail_ratings = std::accumulate(std::cbegin(trailheads), std::cend(trailheads), 0ull, [](size_t sum, const auto& trailhead) {
        return sum + trailhead.rating;
    });

    cout << "\nSum of trail scores = " << sum_of_trail_scores
         << "\nSum of trail ratings = " << sum_of_trail_ratings << endl;
}
