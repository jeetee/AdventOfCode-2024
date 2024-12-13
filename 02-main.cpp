#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <algorithm>
#include <execution>
#include <numeric>
#include <unordered_map>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::string_literals::operator""s;
using std::vector;

static void logic(string fileName);

int main()
{
    cout << "Advent of Code 2024: 02\n";
    logic("02-example.txt"s);
    cout << endl;
    logic("02-input.txt"s);
    return 0;
}

static bool is_report_safe(string line);
static bool are_levels_safe(int lvl_1, int lvl_2, bool ascending);
static bool is_report_dampened_safe(string line);

static void logic(string fileName)
{
    unsigned int n_safe_reports {0};
    unsigned int n_safe_reports_dampened {0};

    cout << "Reading " << fileName << " file " << endl;
    std::ifstream input(fileName);
    for (string line; std::getline(input, line);) {
        cout << "." << std::flush; // Progress report

        if (is_report_safe(line))
        {
            ++n_safe_reports;
            ++n_safe_reports_dampened;
        }
        else if (is_report_dampened_safe(line))
        {
            ++n_safe_reports_dampened;
        }
    }

    cout << "\nNumber of safe reports = " << n_safe_reports
         << "\nNumber of dampened safe reports = " << n_safe_reports_dampened << endl;
}

static bool is_report_safe(string line)
{
    auto ssline = std::stringstream(line);
    int lvl_1, lvl_2;
    bool ascending, safe;
    // Read first two levels to assess direction
    ssline >> lvl_1 >> lvl_2;
    ascending = lvl_2 > lvl_1;
    do {
        safe = are_levels_safe(lvl_1, lvl_2, ascending);
        lvl_1 = lvl_2; // Shift to next level comparison
    } while (safe && (ssline >> lvl_2));
    return safe;
}

static bool are_levels_safe(int lvl_1, int lvl_2, bool ascending)
{
    return (ascending
            && (lvl_2 > lvl_1)
            && ((lvl_2 - lvl_1) <= 3)
        )
        || (!ascending
           && (lvl_1 > lvl_2)
           && ((lvl_1 - lvl_2) <= 3)
        );
}

static bool is_report_dampened_safe(string line)
{
    auto ssline = std::stringstream(line);
    vector<int> levels;
    int level;
    while (ssline >> level)
    {
        levels.emplace_back(level);
    }

    vector<int> intervals(levels.size());
    std::adjacent_difference(std::execution::par, std::cbegin(levels), std::cend(levels), std::begin(intervals));
    intervals.erase(intervals.cbegin()); // Adjacent find always starts with a copy of the first element, drop it

    enum class Direction { ASCENDING, DESCENDING, FLAT };
    auto DirectionFromDifference = [](const int difference)
    {
        return (difference > 0) ? Direction::ASCENDING : ((difference < 0) ? Direction::DESCENDING : Direction::FLAT);
    };

    bool dampened = false;
    for (auto interval_1 = std::begin(intervals); interval_1 != std::end(intervals); ++interval_1)
    {
        const auto interval_dir = DirectionFromDifference(*interval_1);
        const auto interval_2 = (std::next(interval_1));

        if (interval_2 != std::end(intervals) && (interval_dir != DirectionFromDifference(*interval_2)))
        { // Different direction
            if (!dampened)
            { // Possible fixes: remove 1st level of first interval, remove level between both intervals, remove last level of 2nd interval
                std::stringstream line_with_first_level_removed, line_with_middle_level_removed, line_with_last_level_removed;
                const size_t first_level_idx_to_remove = std::distance(std::begin(intervals), interval_1);
                for (size_t level_idx = (first_level_idx_to_remove > 0) ? (first_level_idx_to_remove - 1) : 0; level_idx < levels.size(); ++level_idx)
                {
                    if (level_idx != first_level_idx_to_remove)
                    {
                        line_with_first_level_removed << levels[level_idx] << ' ';
                    }
                    if (level_idx != (first_level_idx_to_remove + 1))
                    {
                        line_with_middle_level_removed << levels[level_idx] << ' ';
                    }
                    if (level_idx != (first_level_idx_to_remove + 2))
                    {
                        line_with_last_level_removed << levels[level_idx] << ' ';
                    }
                }
                // No more room for errors now, we are fine if one of our options is fine
                return is_report_safe(line_with_first_level_removed.str())
                    || is_report_safe(line_with_middle_level_removed.str())
                    || is_report_safe(line_with_last_level_removed.str());
            }
            else
            {
                return false;
            }
        }
        // Both intervals have the same direction

        if (interval_dir == Direction::FLAT)
        { // Can't fix two FLATs in a row
            return false;
        }

        if (  ((interval_dir == Direction::ASCENDING)
                && (*interval_1 > 3)
                )
            || ((interval_dir == Direction::DESCENDING)
                && (*interval_1 < -3)
                )
            )
        { // Interval too large and direction of interval is equal
            if (dampened)
            {
                return false;
            }

            // ==> Assimilating the interval will only lead to a bigger interval
            // This can only be fixed if the interval is either the first or the last one
            if (   (interval_1 == std::begin(intervals))
                || (interval_2 == std::end(intervals))
                )
            { // Ignore this interval (this means either "removing" the first level or the last level of a report
                dampened = true;
                continue;
            }
            else
            { // Removing a level will lead to an even bigger interval
                return false;
            }
        }
        // interval_1 value range is valid
    }

    return true;
}
