#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <algorithm>
#include <execution>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::string_literals::operator""s;
using std::vector;

static void logic(string fileName);

int main()
{
    cout << "Advent of Code 2024: 01\n";
    logic("01-example.txt"s);
    cout << endl;
    logic("01-input.txt"s);
    return 0;
}

static void logic(string fileName)
{
    vector<unsigned int> location_id_list_1;
    vector<unsigned int> location_id_list_2;

    cout << "Reading " << fileName << " file " << endl;
    std::ifstream input(fileName);
    for (string line; std::getline(input, line);) {
        cout << "." << std::flush; // Progress report
        auto ssline = std::stringstream(line);
        unsigned int id_1, id_2;
        ssline >> id_1 >> id_2;
        location_id_list_1.emplace_back(id_1);
        location_id_list_2.emplace_back(id_2);
    }

    std::sort(std::execution::par, std::begin(location_id_list_1), std::end(location_id_list_1));
    std::sort(std::execution::par, std::begin(location_id_list_2), std::end(location_id_list_2));
    // Part 1: sum the distance between both lists
    unsigned long sum = std::transform_reduce(std::execution::par, std::begin(location_id_list_1), std::end(location_id_list_1),
                                              std::begin(location_id_list_2), 0UL, std::plus{}, [](auto id_1, auto id_2){
                                                  return std::max(id_1, id_2) - std::min(id_1, id_2);
                                              });
    cout << "\nTotal distance between lists = " << sum << endl;

    // Part 2: frequency scoring
    auto list_1_it = std::cbegin(location_id_list_1);
    auto list_2_it = std::cbegin(location_id_list_2);
    unsigned long frequency_score = 0UL;
    while (list_1_it != std::cend(location_id_list_1)) {
        auto list_1_value_end = std::equal_range(list_1_it, std::cend(location_id_list_1), *list_1_it).second;
        auto list_2_value_range = std::equal_range(list_2_it, std::cend(location_id_list_2), *list_1_it);
        frequency_score += *list_1_it * (list_2_value_range.second - list_2_value_range.first) * (list_1_value_end - list_1_it);
        list_1_it = list_1_value_end;
    }

    cout << "\nFrequency score between lists = " << frequency_score << endl;
}
