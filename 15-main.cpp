#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <algorithm>
#include <array>
#include <execution>
#include <functional>
#include <list>
#include <limits>
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

typedef struct Point_s {
    long long x;
    long long y;

    struct Point_s operator+(const struct Point_s& other) const
    {
        return Point_s {
            this->x + other.x,
            this->y + other.y,
        };
    }
    struct Point_s& operator+=(const struct Point_s& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
} Point_t;

int main()
{
    cout << "Advent of Code 2024: 15\n";
    logic("15-example.txt"s);
    cout << endl;
    logic("15-example-2.txt"s);
    cout << endl;
    logic("15-input.txt"s);
    return 0;
}

template <typename T>
struct PairHasher {
    std::size_t operator()(const std::pair<T, T>& val) const {
        std::stringstream ss;
        ss << val.first << '|' << val.second;
        return std::hash<string>{}(ss.str());
    }
};

static void logic(string fileName)
{
    vector<string> warehouse;
    const char WALL = '#';
    const char BOX = 'O';
    const char ROBOT = '@';
    const char FLOOR = '.';
    Point_t robot_pos{ 0, 0 };

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    for (string line; std::getline(input, line) && !line.empty();) {
        cout << "." << std::flush; // Progress report
        auto robot_x = line.find(ROBOT);
        if (robot_x != string::npos) {
            robot_pos.x = robot_x;
            robot_pos.y = warehouse.size();
        }
        warehouse.emplace_back(line);
    }

    // PART 1 - simulate movements
    cout << "\nProcessing movements ";
    const std::unordered_map<char, Point_t> directional_offset{
        { '>', { 1, 0 } },
        { '<', { -1, 0 } },
        { '^', { 0, -1 } },
        { 'v', { 0, 1 } },
    };
    std::function<bool(const Point_t&, const Point_t&)> do_move = [&](const Point_t& position, const Point_t& direction) {
        const Point_t next_position = position + direction;
        // Bounds checking
        if ((next_position.y < 0)
            || (static_cast<size_t>(next_position.y) >= warehouse.size())
            || (next_position.x < 0)
            || (static_cast<size_t>(next_position.x) >= warehouse[next_position.y].size())
            )
        {
            return false;
        }
        const char& next = warehouse[next_position.y][next_position.x];
        if ((next == FLOOR)
            || ((next == BOX)
                && do_move(next_position, direction)
                )
            )
        { // We can move
            warehouse[next_position.y][next_position.x] = warehouse[position.y][position.x];
            warehouse[position.y][position.x] = FLOOR;
            return true;
        }
        return false;
    };
    for (string line; std::getline(input, line);) {
        cout << "." << std::flush; // Progress report
        for (const char& direction : line) {
            if (do_move(robot_pos, directional_offset.at(direction)))
            {
                robot_pos += directional_offset.at(direction);
            }
        }
    }

    // The GPS coordinate of a box is equal to 100 times its distance from the top edge of the map plus its distance from the left edge of the map
    unsigned long long sum{ 0 };
    for (size_t y = warehouse.size(); y --> 0;) {
        for (size_t x = warehouse[y].size(); x --> 0;) {
            if (warehouse[y][x] == BOX) {
                sum += 100ull * y + x;
            }
        }
    }

    cout << "\nSum of boxes GPS values = " << sum << endl;
}
