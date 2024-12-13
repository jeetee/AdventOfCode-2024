#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <algorithm>
#include <array>
#include <execution>
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

struct Button_s {
    const unsigned int cost;
    long xOffset;
    long yOffset;
};

struct Point_s {
    long long x;
    long long y;
};

class Machine
{
private:
    Button_s _A{ /*.cost = */3 };
    Button_s _B{ /*.cost = */1 };
    Point_s _Prize{};
public:
    Machine(std::istream& input);
    
    [[nodiscard]] unsigned long long CalculateMinimalPrizeCost(long long prizeOffset = 0) const noexcept;
};

int main()
{
    cout << "Advent of Code 2024: 13\n";
    logic("13-example.txt"s);
    cout << endl;
    logic("13-input.txt"s);
    return 0;
}

struct SizeTPairHasher {
    std::size_t operator()(const std::pair<size_t, size_t>& val) const {
        std::stringstream ss;
        ss << val.first << '|' << val.second;
        return std::hash<string>{}(ss.str());
    }
};

static void logic(string fileName)
{
    vector<Machine> machines;

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    do {
        machines.emplace_back(input);
        cout << '.' << std::flush; // Progress report
    } while (!input.ignore(std::numeric_limits<std::streamsize>::max(), '\n').eof());

    // PART 1
    unsigned long long total_cost = std::accumulate(std::cbegin(machines), std::cend(machines), 0ull, [](unsigned long long sum, const auto& machine) {
        return sum + machine.CalculateMinimalPrizeCost();
    });

    // PART 2
    unsigned long long total_cost_offset = std::accumulate(std::cbegin(machines), std::cend(machines), 0ull, [](unsigned long long sum, const auto& machine) {
        return sum + machine.CalculateMinimalPrizeCost(10000000000000ll);
    });

    cout << "\nTotal minimal cost = " << total_cost
        << "\nTotal cost for prizes offset by 10000000000000 " << total_cost_offset << endl;
}

Machine::Machine(std::istream& input)
{
    input.ignore(std::numeric_limits<std::streamsize>::max(), '+');
    input >> _A.xOffset;
    input.ignore(std::numeric_limits<std::streamsize>::max(), '+');
    input >> _A.yOffset;

    input.ignore(std::numeric_limits<std::streamsize>::max(), '+');
    input >> _B.xOffset;
    input.ignore(std::numeric_limits<std::streamsize>::max(), '+');
    input >> _B.yOffset;

    input.ignore(std::numeric_limits<std::streamsize>::max(), '=');
    input >> _Prize.x;
    input.ignore(std::numeric_limits<std::streamsize>::max(), '=');
    input >> _Prize.y;
}

unsigned long long Machine::CalculateMinimalPrizeCost(long long prizeOffset/* = 0*/) const noexcept
{
    // We need to find out how many button presses of A and B are required to reach the Prize
    // This provides us with the following system of equations to solve:
    //     Prize.x = A * A.xOffset + B * B.xOffset
    //     Prize.y = A * A.yOffset + B * B.yOffset
    // To solve this system, first take top one and solve for A
    //     A * Ax = Px - B * Bx
    // <=> A = (Px - B * Bx) / Ax
    // Substitution in the 2nd equation and solve for B
    //     Py = (Px - B * Bx) / Ax * Ay + B * By
    // <=> Ax * Py = Px * Ay - B * Bx * Ay + B * By * Ax
    // <=> B * Ay * Bx - B * Ax * By = Ay * Px - Ax * Py
    // <=> B * (Ay * Bx - Ax * By) = Ay * Px - Ax * Py
    // <=> B = (Ay * Px - Ax * Py) / (Ay * Bx - Ax * By)
    const auto B_denominator = (_A.yOffset * _B.xOffset) - (_A.xOffset * _B.yOffset);
    if (B_denominator == 0) { // Can't reach prize
        return 0;
    }
    const auto B_nominator = (_A.yOffset * (_Prize.x + prizeOffset)) - (_A.xOffset * (_Prize.y + prizeOffset));
    const auto B = B_nominator / B_denominator;
    // Ensure B was a clean division
    if (B * B_denominator != B_nominator) {
        return 0; // Can't reach prize with button presses
    }

    const auto A_nominator = (_Prize.x + prizeOffset) - (B * _B.xOffset);
    const auto A = A_nominator / _A.xOffset;
    // Ensure A was a clean division
    if (A * _A.xOffset != A_nominator) {
        return 0; // Can't reach prize with button presses
    }

    if ((A < 0) || (B < 0)) {
        return 0; // Can't press a button a negative number of times
    }

    // Found A & B number of presses
    return static_cast<unsigned long long>(A) * _A.cost + static_cast<unsigned long long>(B) * _B.cost;
}
