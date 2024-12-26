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
    std::pair<string, string> MoveTo(char key);
};

static vector<string> get_shortest_button_sequences(string buttons, Keypad& keypad)
{
    vector<string> sequences{ ""s };
    for (char button : buttons) {
        auto buttonSeqs = keypad.MoveTo(button);
        if (!buttonSeqs.second.empty()) {
            auto dupSequences = sequences;
            std::for_each(std::execution::par, std::begin(dupSequences), std::end(dupSequences), [&buttonSeqs](auto& seq) {
                seq += buttonSeqs.second + 'A';
            });
            std::for_each(std::execution::par, std::begin(sequences), std::end(sequences), [&buttonSeqs](auto& seq) {
                seq += buttonSeqs.first + 'A';
            });
            sequences.insert(std::end(sequences), std::begin(dupSequences), std::end(dupSequences));
        }
        else {
            // Only one sequence returned, append it
            std::for_each(std::execution::par, std::begin(sequences), std::end(sequences), [&buttonSeqs](auto& seq) {
                seq += buttonSeqs.first + 'A';
            });
        }
    }
    return sequences;
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
        vector<string> depressurized_robot = get_shortest_button_sequences(line, numpad);

        vector<string> radiation_robot;
        for (auto& seq : depressurized_robot) {
            dirpad.Reset();
            auto seqs = get_shortest_button_sequences(seq, dirpad);
            if (radiation_robot.empty() || (seqs.front().size() < radiation_robot.front().size())) {
                radiation_robot.clear();
                radiation_robot = seqs;
            }
            else if (seqs.front().size() == radiation_robot.front().size()) {
                radiation_robot.insert(std::end(radiation_robot), std::begin(seqs), std::end(seqs));
            }
            // else resulting sequences are longer than those we already had for this level
        }
        
        vector<string> negative_temp_robot;
        for (auto& seq : radiation_robot) {
            dirpad.Reset();
            auto seqs = get_shortest_button_sequences(seq, dirpad);
            if (negative_temp_robot.empty() || (seqs.front().size() < negative_temp_robot.front().size())) {
                negative_temp_robot.clear();
                negative_temp_robot = seqs;
            }
            else if (seqs.front().size() == negative_temp_robot.front().size()) {
                negative_temp_robot.insert(std::end(negative_temp_robot), std::begin(seqs), std::end(seqs));
            }
            // else resulting sequences are longer than those we already had for this level
        }

        cout << line << ":\n"
             << depressurized_robot.front() << "\n"
             << radiation_robot.front() << "\n"
             << negative_temp_robot.front() << endl;

        unsigned long long complexity;
        // Extract numeric value
        std::stringstream ss(line);
        ss >> complexity;
        complexity_sum += complexity * negative_temp_robot.front().size();
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

std::pair<string, string> Keypad::MoveTo(char key)
{
    Position target{};
    for (int y = static_cast<int>(pad.size()); y-- > 0;) {
        for (int x = static_cast<int>(pad[y].size()); x-- > 0;) {
            if (pad[y][x] == key) {
                target = { x, y };
            }
        }
    }

    std::pair<string, string> moves{ ""s, ""s };
    std::stringstream xmoves, ymoves;
    const char moveX = (target.x > current_pos.x) ? '>' : '<';
    const char moveY = (target.y > current_pos.y) ? 'v' : '^';
    for (int diffX = abs(target.x - current_pos.x); diffX-- > 0;) {
        xmoves << moveX;
    }
    for (int diffY = abs(target.y - current_pos.y); diffY-- > 0;) {
        ymoves << moveY;
    }
    if ((xmoves.tellp() == 0) || (ymoves.tellp() == 0)) {
        // Only moving in one direction, order thus is irrelevant
        moves.first = xmoves.str() + ymoves.str();
    }
    else if ((target.x == gap.x)
        && (((gap.y >= current_pos.y) && (gap.y <= target.y))
            || ((gap.y >= target.y) && (gap.y <= current_pos.y))
            )
        )
    { // Move Y first, to avoid crossing gap
        moves.first = ymoves.str() + xmoves.str();
    }
    else if ((target.y == gap.y)
        && (((gap.x >= current_pos.x) && (gap.x <= target.x))
            || ((gap.x >= target.x) && (gap.x <= current_pos.x))
            )
        )
    { // Move X first, to avoid crossing gap
        moves.first = xmoves.str() + ymoves.str();
    }
    else {
        // Order of movement is irrelevant for this level of movement
        moves.first = xmoves.str() + ymoves.str();
        moves.second = ymoves.str() + xmoves.str();
    }

    current_pos = target;
    return moves;
}
