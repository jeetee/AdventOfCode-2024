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
    cout << "Advent of Code 2024: 24\n";
    logic("24-example.txt"s);
    cout << endl;
    logic("24-example-2.txt"s);
    cout << endl;
    logic("24-input.txt"s);
    return 0;
}

struct Gate {
    string name;
    string inputA;
    string inputB;
    enum Operand {
        AND, OR, XOR,
    };
    Operand op;
    bool resolved;
    bool value;
};

static void logic(string fileName)
{
    std::unordered_map<string/*name*/, Gate> gates;

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    cout << "Known inputs ";
    for (string line; std::getline(input, line) && !line.empty();) {
        cout << '.' << std::flush; // Progress report
        std::stringstream ss(line);
        Gate gate;
        std::getline(ss, gate.name, ':');
        int value;
        ss >> value;
        gate.value = (value == 1);
        gate.resolved = true;
        gates.emplace(gate.name, gate);
    }
    cout << "\nGate configurations ";
    for (string line; std::getline(input, line);) {
        cout << '.' << std::flush; // Progress report
        std::stringstream ss(line);
        Gate gate;
        string operandString;
        ss >> gate.inputA >> operandString >> gate.inputB;
        ss.ignore(std::numeric_limits<std::streamsize>::max(), '>');
        ss >> gate.name;
        gate.op = (operandString[0] == 'A') ? Gate::AND : (operandString[0] == 'O') ? Gate::OR : Gate::XOR;
        gate.resolved = false;
        gates.emplace(gate.name, gate);
    }

    // PART 1
    cout << "\nResolving all gates...";
    std::function<void(Gate&)> resolveGate = [&gates, &resolveGate](Gate& gate) {
        if (gate.resolved) {
            return;
        }
        Gate& inputA = gates[gate.inputA];
        if (!inputA.resolved) {
            resolveGate(inputA);
        }
        Gate& inputB = gates[gate.inputB];
        if (!inputB.resolved) {
            resolveGate(inputB);
        }
        switch (gate.op) {
            case Gate::AND: gate.value = inputA.value && inputB.value;
                break;
            case Gate::OR: gate.value = inputA.value || inputB.value;
                break;
            case Gate::XOR: gate.value = inputA.value != inputB.value;
                break;
        }
        gate.resolved = true;
    };
    std::for_each(std::begin(gates), std::end(gates), [&resolveGate](auto& gateEntry) {
        resolveGate(gateEntry.second);
    });
    
    cout << "\nExtracting z-gates...";
    vector<std::pair<string, Gate>> zoutputs;
    std::copy_if(std::cbegin(gates), std::cend(gates), std::back_inserter(zoutputs), [](const auto& gateEntry) {
        return gateEntry.first[0] == 'z';
    });
    std::sort(std::begin(zoutputs), std::end(zoutputs), [](const auto& gateEntryA, const auto& gateEntryB) {
        return gateEntryA.first < gateEntryB.first;
    });
    size_t zNumber = std::accumulate(std::cbegin(zoutputs), std::cend(zoutputs), 0ull, [](size_t acc, const auto& gateEntry) {
        if (gateEntry.second.value) {
            std::stringstream ss(gateEntry.first);
            ss.ignore();
            size_t bitIdx;
            ss >> bitIdx;
            acc |= 1ull << bitIdx;
        }
        return acc;
    });

    cout << "\nResulting number = " << zNumber << endl;
}
