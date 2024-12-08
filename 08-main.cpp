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
    cout << "Advent of Code 2024: 08\n";
    logic("08-example.txt"s);
    cout << endl;
    logic("08-input.txt"s);
    return 0;
}

typedef struct Point_s
{
    size_t x;
    size_t y;

    bool operator==(const struct Point_s& p) const
    {
        return (x == p.x) && (y == p.y);
    }

    struct Point_s operator+(const struct Point_s& p) const
    {
        return Point_s { x + p.x, y + p.y };
    }
    struct Point_s operator-(const struct Point_s& p) const
    {
        return Point_s { x - p.x, y - p.y };
    }
    void operator+=(const struct Point_s& p)
    {
        x += p.x;
        y += p.y;
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

static void logic(string fileName)
{
    const char FLOOR = '.';
    std::unordered_map<char, vector<Point_t>> antennas;

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    size_t width, height, x {0}, y {0};
    for (string line; std::getline(input, line); ++y) {
        width = line.size();
        cout << "." << std::flush; // Progress report
        for (x = line.size(); x --> 0;) {
            if (line[x] != FLOOR) {
                antennas[line[x]].emplace_back(Point_t{x, y});
            }
        }
    }
    height = y;

    // PART 1 + 2
    cout << "\nLocating antinodes ";
    std::unordered_set<Point_t, PointClassHash> antinodelocations, all_antinodes;
    for (const auto& antennalocations : antennas) {
        cout << antennalocations.first << std::flush;
        for (auto it = std::cbegin(antennalocations.second); it != std::prev(std::cend(antennalocations.second)); ++it) {
            std::for_each(std::next(it), std::cend(antennalocations.second), [&](const auto& point){
                cout << '.' << std::flush;
                const Point_t diff { it->x - point.x, it->y - point.y };
                Point_t anti = *it + diff;
                if ((anti.x >= 0) && (anti.x < width) && (anti.y >= 0) && (anti.y < height))
                {
                    antinodelocations.emplace(anti);
                }
                anti = point - diff;
                if ((anti.x >= 0) && (anti.x < width) && (anti.y >= 0) && (anti.y < height))
                {
                    antinodelocations.emplace(anti);
                }

                // PART 2, repeating in both directions
                anti = *it;
                do {
                    all_antinodes.emplace(anti);
                    anti = anti - diff;
                } while ((anti.x >= 0) && (anti.x < width) && (anti.y >= 0) && (anti.y < height));
                for (anti = *it + diff; (anti.x >= 0) && (anti.x < width) && (anti.y >= 0) && (anti.y < height); anti += diff) {
                    all_antinodes.emplace(anti);
                }
            });
        };
    }

    cout << "\nUnique antinode locations = " << antinodelocations.size()
         << "\nUnique repeating antinode locations = " << all_antinodes.size() << endl;
}
