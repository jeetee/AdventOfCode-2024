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
    cout << "Advent of Code 2024: 21\n";
    logic("21-example.txt"s);
    cout << endl;
    logic("21-input.txt"s);
    return 0;
}

struct Position {
    int x;
    int y;
};

class Keypad
{
private:
    vector<string> pad;
    Position current_pos;
    Position initial_pos{};
    Position gap{};
public:
    Keypad(vector<string> keypad);
    void Reset();
    string MoveTo(char key);
};

string get_button_sequence(string buttons, Keypad& keypad)
{
    string sequence{};
    for (char button : buttons) {
        sequence += keypad.MoveTo(button) + 'A';
    }
    return sequence;
}

static void logic(string fileName)
{
    Keypad numpad({
        {'7', '8', '9'},
        {'4', '5', '6'},
        {'1', '2', '3'},
        {' ', '0', 'A'},
    });
    Keypad dirpad({
        {' ', '^', 'A'},
        {'<', 'v', '>'},
    });
    unsigned long long complexity_sum{ 0 };

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    for (string line; std::getline(input, line);) {
        numpad.Reset();
        string depressurized_robot = get_button_sequence(line, numpad);
        dirpad.Reset();
        string radiation_robot = get_button_sequence(depressurized_robot, dirpad);
        dirpad.Reset();
        string negative_temp_robot = get_button_sequence(radiation_robot, dirpad);

        cout << line << ":\n"
             << depressurized_robot << "\n"
             << radiation_robot << "\n"
             << negative_temp_robot << endl;

        unsigned long long complexity;
        // Extract numeric value
        std::stringstream ss(line);
        ss >> complexity;
        complexity_sum += complexity * negative_temp_robot.size();
    }

    cout << "\nComplexity Sum = " << complexity_sum << endl;
}

Keypad::Keypad(vector<string> keypad)
{
    pad = keypad;
    for (int y = static_cast<int>(pad.size()); y-- > 0;) {
        for (int x = static_cast<int>(pad[y].size()); x-- > 0;) {
            if (pad[y][x] == 'A') {
                initial_pos = { x, y };
            }
            else if (pad[y][x] == ' ') {
                gap = { x, y };
            }
        }
    }
    current_pos = initial_pos;
}

void Keypad::Reset()
{
    current_pos = initial_pos;
}

string Keypad::MoveTo(char key)
{
    Position target{};
    for (int y = static_cast<int>(pad.size()); y-- > 0;) {
        for (int x = static_cast<int>(pad[y].size()); x-- > 0;) {
            if (pad[y][x] == key) {
                target = { x, y };
            }
        }
    }

    std::stringstream moves;
    char moveY = (target.y > current_pos.y) ? 'v' : '^';
    char moveX = (target.x > current_pos.x) ? '>' : '<';
    if (target.x == gap.x) {
        // Move Y first, to avoid crossing gap
        for (int diffY = abs(target.y - current_pos.y); diffY --> 0;) {
            moves << moveY;
        }
        // Then move X
        for (int diffX = abs(target.x - current_pos.x); diffX --> 0;) {
            moves << moveX;
        }
    }
    else {
        // Move X first, to avoid crossing gap
        for (int diffX = abs(target.x - current_pos.x); diffX-- > 0;) {
            moves << moveX;
        }
        // Then move Y
        for (int diffY = abs(target.y - current_pos.y); diffY-- > 0;) {
            moves << moveY;
        }
    }

    current_pos = target;
    return moves.str();
}
