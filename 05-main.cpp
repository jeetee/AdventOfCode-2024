#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <algorithm>
#include <execution>
#include <vector>
#include <unordered_map>

using std::cout;
using std::endl;
using std::string;
using std::string_literals::operator""s;
using std::vector;
using std::unordered_map;

static void logic(string fileName);

int main()
{
    cout << "Advent of Code 2024: 05\n";
    logic("05-example.txt"s);
    cout << endl;
    logic("05-input.txt"s);
    return 0;
}

static void logic(string fileName)
{
    unordered_map<unsigned int, vector<unsigned int>> page_order_rules;

    cout << "Reading rules from " << fileName << endl;
    std::ifstream input(fileName);
    for (string line; std::getline(input, line) && line.size();) {
        cout << "." << std::flush; // Progress report
        std::stringstream ss(line);
        unsigned int page, later_page;
        ss >> page; ss.ignore(); ss >> later_page;
        page_order_rules[page].emplace_back(later_page);
    }
    cout << "\nProcessing updates" << endl;

    unsigned long middle_sum{ 0ul };
    unsigned long middle_sum_corrected{ 0ul };
    for (string line; std::getline(input, line);) {
        cout << "." << std::flush; // Progress report
        vector<unsigned int> pages_in_update;
        bool update_valid = true;

        std::stringstream ss(line);
        for (unsigned int page; (ss >> page); ss.ignore())
        {
            // PART 1: check if there are no pages preceding me that should be after me
            update_valid &= std::none_of(std::execution::par, std::cbegin(page_order_rules[page]), std::cend(page_order_rules[page]), [&](const auto& later_page) {
                return std::find(std::execution::par, std::cbegin(pages_in_update), std::cend(pages_in_update), later_page) != std::cend(pages_in_update);
            });
            pages_in_update.emplace_back(page);
        }
        if (update_valid)
        {
            middle_sum += pages_in_update[pages_in_update.size() / 2];
        }
        else
        {
            // PART 2: re-order the update according to the page rules
            std::sort(std::begin(pages_in_update), std::end(pages_in_update), [&page_order_rules](const auto& left, const auto& right) {
                return std::find(std::execution::par, std::cbegin(page_order_rules[left]), std::cend(page_order_rules[left]), right) != std::cend(page_order_rules[left]);
            });
            middle_sum_corrected += pages_in_update[pages_in_update.size() / 2];
        }
    }

    cout << "\nSum of middle pages of valid updates = " << middle_sum
         << "\nSum of middle pages of corrected updates = " << middle_sum_corrected << endl;
}
