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
    cout << "Advent of Code 2024: 06\n";
    logic("06-example.txt"s);
    cout << endl;
    logic("06-input.txt"s);
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

enum class Direction
{
    UP, DOWN, LEFT, RIGHT
};
struct EnumClassHash // Allows using an enum class as the key value in (unordered)maps
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};
static Direction rotate90degRight(const Direction& dir)
{
    static const unordered_map<const Direction, const Direction, EnumClassHash> rotRight {
        { Direction::UP,    Direction::RIGHT },
        { Direction::DOWN,  Direction::LEFT  },
        { Direction::LEFT,  Direction::UP    },
        { Direction::RIGHT, Direction::DOWN  },
    };
    return rotRight.at(dir);
}
static Direction charToDirection(const char& c)
{
    static const unordered_map<const char, const Direction, EnumClassHash> charToDir {
        { '>', Direction::RIGHT },
        { '<', Direction::LEFT  },
        { '^', Direction::UP    },
        { 'v', Direction::DOWN  },
    };
    return charToDir.at(c);
}
static string_view directionToSV(const Direction& dir)
{
    static const unordered_map<const Direction, const string_view, EnumClassHash> dirToSv {
        { Direction::UP,    "up"sv },
        { Direction::DOWN,  "down"sv },
        { Direction::LEFT,  "left"sv },
        { Direction::RIGHT, "right"sv },
    };
    return dirToSv.at(dir);
}

static void logic(string fileName)
{
    vector<string> lines;
    struct { Point_t pos; Direction dir; } guard;
    const char FLOOR = '.';

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    size_t x {0}, y {0};
    for (string line; std::getline(input, line); ++y) {
        cout << "." << std::flush; // Progress report
        x = line.find_first_not_of(".#"s);
        if (x != string::npos)
        {
            guard.pos = Point_t {x, y};
            guard.dir = charToDirection(line[x]);
            // Remove guard from grid, so we can walk over this space if needed in the future
            line[x] = FLOOR;
        }
        lines.emplace_back(line);
    }
    cout << "\nFound guard starting position = " << guard.pos.x << ", " << guard.pos.y << " facing " << directionToSV(guard.dir) << std::flush;

    // PART 1
    const size_t height = lines.size();
    const size_t width = lines.front().size();
    unordered_set<Point_t, PointClassHash> visited_positions { guard.pos };
    for (;;)
    { // Movement loop, will break from the inside when guard leaves the area
        const Point_t next_position {
            guard.pos.x + ((guard.dir == Direction::LEFT) ? -1 : ((guard.dir == Direction::RIGHT) ? 1 : 0)),
            guard.pos.y + ((guard.dir == Direction::UP  ) ? -1 : ((guard.dir == Direction::DOWN ) ? 1 : 0))
        };

        if (   (next_position.x < 0) || (next_position.x >= width)
            || (next_position.y < 0) || (next_position.y >= height)
            )
        { // Outside the grid => guard left the area, we are done
            break;
        }

        if (lines[next_position.y][next_position.x] != FLOOR)
        { // Going to hit an obstacle
            guard.dir = rotate90degRight(guard.dir);
            // Didn't move yet, try again in the new direction
            continue;
        }

        // Valid new position, apply and trace it
        guard.pos = next_position;
        visited_positions.emplace(guard.pos);
    }

    cout << "\nGuard left the area after visiting = " << visited_positions.size() << " positions." << endl;
}
