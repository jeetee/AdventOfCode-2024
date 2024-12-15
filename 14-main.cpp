#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <algorithm>
#include <array>
#include <execution>
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

static void logic(string fileName, const int width, const int height);

typedef struct Point_s {
    long long x;
    long long y;
} Point_t;

class Robot
{
private:
    Point_t _pos{};
    Point_t _velocity{};
public:
    Robot(std::istream& input);
    
    [[nodiscard]] Point_t GetPosAfterXTime(unsigned int time, size_t width, size_t height) const noexcept;
};

int main()
{
    cout << "Advent of Code 2024: 14\n";
    logic("14-example.txt"s, 11, 7);
    cout << endl;
    logic("14-input.txt"s, 101, 103);
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

static void logic(string fileName, const int width, const int height)
{
    vector<Robot> robots;

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    for (string line; std::getline(input, line);) {
        cout << "." << std::flush; // Progress report
        std::stringstream ss(line);
        robots.emplace_back(ss);
        cout << '.' << std::flush; // Progress report
    }

    // PART 1
    vector<Point_t> resulting_robot_positions;
    std::for_each(std::execution::seq, std::cbegin(robots), std::cend(robots), [&](const auto& robot) {
        resulting_robot_positions.emplace_back(robot.GetPosAfterXTime(100, width, height));
    });
    std::array<std::pair<Point_t, Point_t>, 4> quadrants{{ // ranges are [topLeft, bottomRight(
        {{ 0,               0                }, { width / 2, height / 2 }}, // Top Left
        {{ (width / 2) + 1, 0                }, { width    , height / 2 }}, // Top Right
        {{ 0              , (height / 2) + 1 }, { width / 2, height }}, // Bottom Left
        {{ (width / 2) + 1, (height / 2) + 1 }, { width    , height }}, // Bottom Right
    }};
    std::array<size_t, 4> robots_per_quadrant{};
    std::transform(std::execution::par, std::cbegin(quadrants), std::cend(quadrants), std::begin(robots_per_quadrant), [&resulting_robot_positions](const auto& quadrant) {
        return std::count_if(std::execution::par, std::cbegin(resulting_robot_positions), std::cend(resulting_robot_positions), [&quadrant](const auto& result_pos) {
            return (result_pos.x >= quadrant.first.x)
                && (result_pos.x < quadrant.second.x)
                && (result_pos.y >= quadrant.first.y)
                && (result_pos.y < quadrant.second.y);
        });
    });

    unsigned long long safety_factor = std::accumulate(std::cbegin(robots_per_quadrant), std::cend(robots_per_quadrant), 1ull, std::multiplies<unsigned long long>());

    cout << "\nSafety factor = " << safety_factor << endl;
}

Robot::Robot(std::istream& input)
{
    input.ignore(std::numeric_limits<std::streamsize>::max(), '=');
    input >> _pos.x;
    input.ignore();
    input >> _pos.y;

    input.ignore(std::numeric_limits<std::streamsize>::max(), '=');
    input >> _velocity.x;
    input.ignore();
    input >> _velocity.y;
}

Point_t Robot::GetPosAfterXTime(unsigned int time, size_t width, size_t height) const noexcept
{
    Point_t new_pos{
        _pos.x + time * _velocity.x,
        _pos.y + time * _velocity.y
    };
    // Robots teleport to "wrap around" when they would fall outside of the area
    if (new_pos.x < 0) {
        new_pos.x += ((abs(new_pos.x) + width - 1) / width) * width; // Ceiled division of the absolute value (prevents +1 in case of clean division)
    }
    else if (static_cast<size_t>(new_pos.x) >= width) {
        new_pos.x %= width;
    }
    if (new_pos.y < 0) {
        new_pos.y += ((abs(new_pos.y) + height - 1) / height) * height; // Ceiled division of the absolute value (prevents +1 in case of clean division)
    }
    else if (static_cast<size_t>(new_pos.y) >= height) {
        new_pos.y %= height;
    }
    return new_pos;
}
