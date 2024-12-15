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
static void logic_part_2(string fileName);

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
    cout << endl
         << "Part 2" << endl;
    logic_part_2("15-example-2.txt"s);
    cout << endl;
    logic_part_2("15-input.txt"s);
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

static void logic_part_2(string fileName)
{
    vector<string> warehouse;
    static constexpr char WALL = '#';
    static constexpr char BOX = 'O';
    static constexpr char ROBOT = '@';
    static constexpr char FLOOR = '.';
    static constexpr char BOX_LEFT = '[';
    static constexpr char BOX_RIGHT = ']';
    Point_t robot_pos{ 0, 0 };

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    for (string line; std::getline(input, line) && !line.empty();) {
        cout << "." << std::flush; // Progress report
        std::stringstream expanded_line;
        for (const char& c : line) {
            switch (c)
            {
            case WALL: expanded_line << WALL << WALL;
                break;
            case BOX: expanded_line << BOX_LEFT << BOX_RIGHT;
                break;
            case ROBOT:{
                    robot_pos.x = expanded_line.str().size();
                    robot_pos.y = warehouse.size();
                    expanded_line << ROBOT << FLOOR;
                }
                break;
            case FLOOR: expanded_line << FLOOR << FLOOR;
                break;
            }
        }
        warehouse.emplace_back(expanded_line.str());
    }

    // PART 2 - simulate movements
    cout << "\nProcessing movements ";
    const std::unordered_map<char, Point_t> directional_offset{
        { '>', { 1, 0 } },
        { '<', { -1, 0 } },
        { '^', { 0, -1 } },
        { 'v', { 0, 1 } },
    };
    // Optimized function for left & right movement
    std::function<bool(const Point_t&, const Point_t&)> move_horizontally = [&](const Point_t& position, const Point_t& direction) {
        // Bounds checking
        if (((position.x == 0) && (direction.x < 0))
            || ((position.x == warehouse[position.y].size() - 1) && (direction.x > 0))
            ) {
            return false;
        }

        Point_t end_position = position + direction;
        while ((end_position.x >= 0) && (static_cast<size_t>(end_position.x) < warehouse[position.y].size()))
        {
            switch (warehouse[position.y][end_position.x]) {
                case FLOOR: {
                    // Can move, do so
                    while (end_position.x != position.x)
                    {
                        warehouse[position.y][end_position.x] = warehouse[position.y][end_position.x - direction.x];
                        end_position.x -= direction.x;
                    }
                    // Our original position is now cleared
                    warehouse[position.y][position.x] = FLOOR;
                    return true;
                }
                break;

                case WALL: {
                    return false;
                }
                break;

                default: // Part of a BOX: No-op, continue looking
                    break;
            }
            end_position += direction;
        }
        // Out of bounds
        return false;
    };
    std::function<bool(const Point_t&, const Point_t&)> can_move_vertically = [&](const Point_t& position, const Point_t& direction) {
        const Point_t next_position = position + direction;
        // Bounds checking
        if ((next_position.y < 0)
            || (static_cast<size_t>(next_position.y) >= warehouse.size())
            )
        {
            return false;
        }
        const char& next = warehouse[next_position.y][next_position.x];
        if ((next == FLOOR)
            || ((next == BOX_LEFT)
                && can_move_vertically(next_position, direction)
                && can_move_vertically(next_position + Point_t{ 1, 0 }, direction) // Need to be able to move the box right side as well
                )
            || ((next == BOX_RIGHT)
                && can_move_vertically(next_position, direction)
                && can_move_vertically(next_position + Point_t{ -1, 0 }, direction) // Need to be able to move the box left side as well
                )
            )
        { // We can move
            return true;
        }
        return false;
    };
    std::function<bool(const Point_t&, const Point_t&, const bool)> move_vertically = [&](const Point_t& position, const Point_t& direction, const bool validate) {
        if (validate && !can_move_vertically(position, direction)) {
            return false;
        }
        // We can move, do it recursively for all affected elements, so we work from tail to start and don't overwrite something
        const Point_t next_position = position + direction;
        const char& next = warehouse[next_position.y][next_position.x];

        if (next == BOX_LEFT) {
            move_vertically(next_position, direction, false);
            // Move the box right side as well
            move_vertically(next_position + Point_t{ 1, 0 }, direction, false);
        } else if (next == BOX_RIGHT) {
            move_vertically(next_position, direction, false);
            // Move the box left side as well
            move_vertically(next_position + Point_t{ -1, 0 }, direction, false);
        }

        // Dependencies have moved, move ourselves
        warehouse[next_position.y][next_position.x] = warehouse[position.y][position.x];
        warehouse[position.y][position.x] = FLOOR;

        return true;
    };

    for (string line; std::getline(input, line);) {
        cout << "." << std::flush; // Progress report
        for (const char& direction : line) {
            const auto& offset = directional_offset.at(direction);
            if (((offset.x != 0) && move_horizontally(robot_pos, offset))
                || ((offset.y != 0) && move_vertically(robot_pos, offset, true))
                )
            {
                robot_pos += offset;
            }
        }
    }

    // The GPS coordinate of a box is equal to 100 times its distance from the top edge of the map plus its distance from the left edge of the map
    unsigned long long sum{ 0 };
    for (size_t y = warehouse.size(); y-- > 0;) {
        for (size_t x = warehouse[y].size(); x-- > 0;) {
            if (warehouse[y][x] == BOX_LEFT) {
                sum += 100ull * y + x;
            }
        }
    }

    cout << "\nSum of wide boxes GPS values = " << sum << endl;
}
