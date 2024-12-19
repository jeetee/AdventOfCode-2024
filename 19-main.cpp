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

static bool can_make_design(const string& design, const vector<string>&towels);

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

    // PART 1
    cout << "Processing Designs ";
    unsigned int possible_design_count{ 0 };
    for (string design; std::getline(input, design);) {
        cout << '.' << std::flush; // Progress indication
        if (can_make_design(design, towels)) {
            ++possible_design_count;
        }
    }

    cout << "\nNumber of possible designs = " << possible_design_count << endl;
}

static bool can_make_design(const string& design, const vector<string>& towels)
{
    // A design can be made if we can progressively make each subpart with a towel and reach the end of the design with an exact size match.
    std::unordered_set<size_t> idxs_to_check{ 0 };
    while (!idxs_to_check.empty()) {
        auto idx_to_start = *(idxs_to_check.cbegin());
        idxs_to_check.erase(idx_to_start);

        for (const string& towel : towels) {
            const auto next_idx = idx_to_start + towel.size();
            if (next_idx > design.size()) {
                continue;
            }
            if (design.substr(idx_to_start, towel.size()) == towel) {
                if (next_idx == design.size()) {
                    // Exact match on total length as well -> found a combo that works
                    return true;
                }
                idxs_to_check.insert(next_idx);
            }
            // else: no match on this towel
        }
    }
    // Did not find a match
    return false;
}
