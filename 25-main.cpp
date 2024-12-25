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
    cout << "Advent of Code 2024: 25\n";
    logic("25-example.txt"s);
    cout << endl;
    logic("25-input.txt"s);
    return 0;
}

static void logic(string fileName)
{
    static constexpr char FILLED = '#';
    static constexpr char EMPTY = '.';
    static constexpr size_t KEY_LOCK_HEIGHT = 5;

    vector<vector<size_t>> keys;
    vector<vector<size_t>> locks;

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    for (string line; std::getline(input, line);) {
        cout << '.' << std::flush; // Progress report
        bool lock_not_key = (line[0] == FILLED);
        vector<size_t> item(line.size(), 0);
        // Read rest of lock/key
        while (std::getline(input, line) && !line.empty()) {
            std::transform(std::execution::par, std::cbegin(line), std::cend(line), std::cbegin(item), std::begin(item), [](const char& in, const size_t current_count) {
                return (in == FILLED) ? (current_count + 1) : current_count;
            });
        }
        if (lock_not_key) {
            locks.push_back(item);
        }
        else {
            // Keys always end with a FILLED row, we don't want to count it
            std::for_each(std::execution::par, std::begin(item), std::end(item), [](size_t& count) {
                --count;
            });
            keys.push_back(item);
        }
    }
    cout << "\nRead " << keys.size() << " keys and " << locks.size() << " locks" << endl;

    // PART 1
    cout << "Comparing keys against locks..." << endl;
    size_t no_overlaps = std::accumulate(std::cbegin(locks), std::cend(locks), 0ull, [&keys](size_t no_overlap_count, const auto& lock) {
        return no_overlap_count + std::count_if(std::execution::par, std::cbegin(keys), std::cend(keys), [&lock](const auto& key) {
            bool no_overlap = (lock.size() == key.size()); // Assume no overlap if lengths are equal
            for (auto i = lock.size(); no_overlap && (i --> 0);) {
                // Verify assumption of no overlap
                no_overlap = (lock[i] + key[i]) <= KEY_LOCK_HEIGHT;
            }
            return no_overlap;
        });
    });
    cout << "Unique key/lock combinations without overlaps = " << no_overlaps << endl;
}
