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
#include <map>
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
    cout << "Advent of Code 2024: 16\n";
    logic("16-example.txt"s);
    cout << endl;
    logic("16-example-2.txt"s);
    cout << endl;
    logic("16-input.txt"s);
    return 0;
}

struct Position {
    size_t x;
    size_t y;

    bool operator==(const struct Position& p) const
    {
        return (x == p.x) && (y == p.y);
    }
    bool operator!=(const struct Position& p) const
    {
        return !(*this == p);
    }
};
struct PositionHasher
{
    std::size_t operator()(const Position& p) const
    {
        std::stringstream ss;
        ss << p.x << '|' << p.y;
        return std::hash<string>{}(ss.str());
    }
};

enum class Direction {
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
static string_view directionToSV(const Direction& dir)
{
    static const unordered_map<const Direction, const string_view, EnumClassHash> dirToSv{
        { Direction::UP,    "up"sv },
        { Direction::DOWN,  "down"sv },
        { Direction::LEFT,  "left"sv },
        { Direction::RIGHT, "right"sv },
    };
    return dirToSv.at(dir);
}
static std::ostream& operator<<(std::ostream& os, const Direction& dir)
{
    os << directionToSV(dir);
    return os;
}

template <typename T>
struct PairHasher {
    std::size_t operator()(const std::pair<T, T>& val) const {
        std::stringstream ss;
        ss << val.first << '|' << val.second;
        return std::hash<string>{}(ss.str());
    }
};

static size_t n_rotations_for(const Direction& dir, const Direction& other_dir)
{
    static const unordered_map<std::pair<const Direction, const Direction>, size_t, PairHasher<const Direction>> rotationMap {
        {{ Direction::UP,    Direction::UP    }, 0},
        {{ Direction::DOWN,  Direction::DOWN  }, 0},
        {{ Direction::LEFT,  Direction::LEFT  }, 0},
        {{ Direction::RIGHT, Direction::RIGHT }, 0},
        {{ Direction::UP,    Direction::LEFT  }, 1},
        {{ Direction::UP,    Direction::RIGHT }, 1},
        {{ Direction::DOWN,  Direction::LEFT  }, 1},
        {{ Direction::DOWN,  Direction::RIGHT }, 1},
        {{ Direction::LEFT,  Direction::UP    }, 1},
        {{ Direction::LEFT,  Direction::DOWN  }, 1},
        {{ Direction::RIGHT, Direction::UP    }, 1},
        {{ Direction::RIGHT, Direction::DOWN  }, 1},
        {{ Direction::UP,    Direction::DOWN  }, 2},
        {{ Direction::DOWN,  Direction::UP    }, 2},
        {{ Direction::LEFT,  Direction::RIGHT }, 2},
        {{ Direction::RIGHT, Direction::LEFT  }, 2},
    };
    return rotationMap.at(std::make_pair(dir, other_dir));
}

struct Step {
    Position pos;
    Direction dir;

    bool operator==(const struct Step& s) const
    {
        return (pos == s.pos) && (dir == s.dir);
    }
    bool operator!=(const struct Step& s) const
    {
        return !(*this == s);
    }
};
struct StepHasher
{
    std::size_t operator()(const Step& s) const
    {
        std::stringstream ss;
        ss << s.pos.x << '|' << s.pos.y << '|' << s.dir;
        return std::hash<string>{}(ss.str());
    }
};

struct StepNode {
    Step step;
    vector<StepNode> previous_step; // More than one route could lead to the same step at the same cost
};
struct StepNodeHasher
{
    std::size_t operator()(const StepNode& sn) const
    {
        std::stringstream ss;
        ss << sn.step.pos.x << '|' << sn.step.pos.y << '|' << sn.step.dir;
        return std::hash<string>{}(ss.str());
    }
};

static void logic(string fileName)
{
    static constexpr char WALL = '#';
    static constexpr char FLOOR = '.';
    static constexpr char START = 'S';
    static constexpr char END = 'E';

    vector<string>maze;
    Step start{};
    Position end{};

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    for (string line; std::getline(input, line);) {
        maze.emplace_back(line);
        for (size_t x = line.size(); x --> 0;) {
            if (line[x] == START) {
                start.pos.x = x;
                start.pos.y = maze.size() - 1;
                start.dir = Direction::RIGHT;
            } else if (line[x] == END) {
                end.x = x;
                end.y = maze.size() - 1;
            }
        }
    }

    // PART 1 + 2
    cout << "\nSolving ";
    std::multimap<unsigned long long /*cost*/, StepNode> steps_to_process{{
        { 0, { start } },
    }};
    unordered_map<Step, unsigned long long /*cost*/, StepHasher> minimal_cost_to_step{{
        { start, 0 },
    }};

    while (!steps_to_process.empty() && (steps_to_process.cbegin()->second.step.pos != end)) {
        cout << '.' << std::flush; // Progress indication

        // Continue from current cheapest path, check directions we can move to
        auto current_step_handle = steps_to_process.extract(steps_to_process.begin());
        static const std::array<Direction, 4> directions = { Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT };

        for (const Direction& dir : directions) {
            size_t rotations_needed = n_rotations_for(current_step_handle.mapped().step.dir, dir);
            // 1000 point per rotation, 1 point for a step in the same direction
            const auto resulting_step_cost = current_step_handle.key() + ((rotations_needed != 0) ? (rotations_needed * 1000) : 1);
            
            Step next_step = current_step_handle.mapped().step;
            switch (dir) {
                case Direction::UP:    next_step.pos.y--; break;
                case Direction::DOWN:  next_step.pos.y++; break;
                case Direction::LEFT:  next_step.pos.x--; break;
                case Direction::RIGHT: next_step.pos.x++; break;
            }
            if ((next_step.pos.y < maze.size())
                && (next_step.pos.x < maze[next_step.pos.y].size())
                && (maze[next_step.pos.y][next_step.pos.x] != WALL)
                )
            { // Valid position for next_step
                auto resulting_step = (rotations_needed == 0) ? next_step : current_step_handle.mapped().step;
                resulting_step.dir = dir; // No-op in case of no rotation
                // Evaluate cost to resulting position
                auto current_min_cost = minimal_cost_to_step[resulting_step];
                if (resulting_step_cost < current_min_cost) {
                    // Cheaper route to it
                    minimal_cost_to_step[resulting_step] = resulting_step_cost;
                    // Locate it in processing queue and update
                    auto possible_next_step_entries_by_cost = steps_to_process.equal_range(current_min_cost);
                    auto step_it = std::find_if(std::execution::par,
                        possible_next_step_entries_by_cost.first, possible_next_step_entries_by_cost.second, [&resulting_step](const auto& step_to_process) {
                            return (step_to_process.second.step == resulting_step);
                        }
                    );
                    if (step_it != possible_next_step_entries_by_cost.second) {
                        auto resulting_stepnode_handle = steps_to_process.extract(step_it);
                        resulting_stepnode_handle.key() = resulting_step_cost;
                        // Our new path to here is cheaper than the existing ones, update this information
                        resulting_stepnode_handle.mapped().previous_step = {
                            current_step_handle.mapped()
                        };
                        steps_to_process.insert(std::move(resulting_stepnode_handle));
                    }
                    else {
                        throw new std::domain_error("Failed to locate existing route while trying to register a cheaper version of it"s);
                    }
                }
                else if ((current_min_cost == 0) && (resulting_step != start)) {
                    // New map entry, didn't exist yet
                    minimal_cost_to_step[resulting_step] = resulting_step_cost;
                    steps_to_process.insert(std::make_pair(resulting_step_cost, StepNode{ resulting_step, { current_step_handle.mapped() } }));
                }
                else if (current_min_cost == resulting_step_cost) {
                    // Different path to resulting node with same cost, add it to the correct step_to_process
                    auto possible_next_step_entries_by_cost = steps_to_process.equal_range(current_min_cost);
                    auto step_it = std::find_if(std::execution::par,
                        possible_next_step_entries_by_cost.first, possible_next_step_entries_by_cost.second, [&resulting_step](const auto& step_to_process) {
                            return (step_to_process.second.step == resulting_step);
                        }
                    );
                    if (step_it != possible_next_step_entries_by_cost.second) {
                        step_it->second.previous_step.emplace_back(current_step_handle.mapped());
                    }
                    else {
                        throw new std::domain_error("Failed to locate existing route while trying to register a cheaper version of it"s);
                    }
                }
                // else -> found, but new path costs more, abandon it
            }
            // else -> invalid position, abandon
        }
    }

    cout << "\nMinimal cost to reach the exit = " << steps_to_process.cbegin()->first << endl;
    unordered_set<Position, PositionHasher> best_path_tiles;
    std::function<void(const StepNode&)> AddStepNodePositionToSet = [&best_path_tiles, &AddStepNodePositionToSet](const StepNode& stepnode) {
        best_path_tiles.insert(stepnode.step.pos);
        for (const auto& from : stepnode.previous_step) {
            AddStepNodePositionToSet(from);
        }
    };
    AddStepNodePositionToSet(steps_to_process.cbegin()->second);
    cout << "Number of tiles part of any optimal path = " << best_path_tiles.size() << endl;
}
