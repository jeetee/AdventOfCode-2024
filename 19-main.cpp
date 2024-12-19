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

int main()
{
    cout << "Advent of Code 2024: 19\n";
    logic("19-example.txt"s);
    cout << endl;
    logic("19-input.txt"s);
    return 0;
}

static size_t can_make_design(const string& design, const vector<string>&towels);

static void logic(string fileName)
{
    vector<string>towels;

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    {
        string line;
        std::getline(input, line);
        std::stringstream ss(line);
        for (string towel; std::getline(ss, towel, ','); ss.ignore()) { // Read next string and skip space following comma afterwards
            towels.emplace_back(towel);
        }
    }
    input.ignore(); // Skip empty separator line

    // PART 1 + 2
    cout << "Processing Designs ";
    unsigned int possible_design_count{ 0 };
    size_t possible_design_count_ways{ 0 };
    for (string design; std::getline(input, design);) {
        cout << '.' << std::flush; // Progress indication
        const auto design_count = can_make_design(design, towels);
        if (design_count > 0) {
            ++possible_design_count;
            possible_design_count_ways += design_count;
        }
    }

    cout << "\nNumber of possible designs = " << possible_design_count
         << "\nNumber of ways to achieve them = " << possible_design_count_ways << endl;
}

static size_t can_make_design(const string& design, const vector<string>& towels)
{
    // A design can be made if we can progressively make each subpart with a towel and reach the end of the design with an exact size match.
    size_t number_of_ways{ 0 };
    std::unordered_map<size_t/*idx*/, size_t/*ways_to_reach_idx*/> idxs_to_check{ {0, 1} };
    while (!idxs_to_check.empty()) {
        // Process from low to high, ensuring all paths to an index are counted
        auto [idx_to_start, count] = *(idxs_to_check.cbegin());
        idxs_to_check.erase(idx_to_start);

        for (const string& towel : towels) {
            const auto next_idx = idx_to_start + towel.size();
            if (next_idx > design.size()) {
                continue;
            }
            if (design.substr(idx_to_start, towel.size()) == towel) {
                if (next_idx == design.size()) {
                    // Exact match on total length as well -> found a combo that works
                    number_of_ways += count;
                }
                else {
                    idxs_to_check[next_idx] += count;
                }
            }
            // else: no match on this towel
        }
    }

    return number_of_ways;
}
