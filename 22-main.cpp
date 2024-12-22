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
    cout << "Advent of Code 2024: 22\n";
    logic("22-example.txt"s);
    cout << endl;
    logic("22-example-2.txt"s);
    cout << endl;
    logic("22-input.txt"s);
    return 0;
}

static unsigned long long evolve(unsigned long long secret)
{
    secret ^= (secret * 64);
    secret %= 16777216ull;
    secret ^= (secret / 32);
    secret %= 16777216ull;
    secret ^= (secret * 2048);
    secret %= 16777216ull;
    return secret;
}

static void logic(string fileName)
{
    vector<unsigned long long> secrets;
    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    for (unsigned long long secret; input >> secret;) {
        secrets.emplace_back(secret);
    }

    // PART 1
    cout << "\nSolving ";
    std::transform(std::execution::par, std::cbegin(secrets), std::cend(secrets), std::begin(secrets), [](unsigned long long secret) {
        for (int count = 2000; count --> 0;) {
            secret = evolve(secret);
        }
        return secret;
    });

    cout << "\nSum of 2000th secret numbers = " << std::accumulate(std::begin(secrets), std::end(secrets), 0ull) << endl;
}
