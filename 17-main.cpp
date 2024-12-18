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

struct CPU_s {
    long long A{ 0 };
    long long B{ 0 };
    long long C{ 0 };

    enum opcode {
        ADV = 0,
        BXL = 1,
        BST = 2,
        JNZ = 3,
        BXC = 4,
        OUT = 5,
        BDV = 6,
        CDV = 7,
    };

    vector<long long> output_buffer;

private:
    long long combo_operand_value(unsigned int operand) const
    {
        if (operand < 4) {
            return operand;
        }
        switch (operand) {
            case 4: return A;
                break;
            case 5: return B;
                break;
            case 6: return C;
                break;
            case 7: [[fallthrough]]; // Reserved, not used in valid programs
            default:
                break;
        }
        throw new std::out_of_range("Invalid operand"s);
    }

    vector<unsigned int> program;
    size_t instruction_pointer{ 0 };

    void process_current_instruction()
    {
        unsigned int instruction, operand;
        instruction = program[instruction_pointer++];
        operand = program[instruction_pointer++];

        switch (instruction) {
        case ADV: A /= static_cast<long long>(pow(2, combo_operand_value(operand)));
            break;
        case BXL: B ^= operand;
            break;
        case BST: B = combo_operand_value(operand) % 8;
            break;
        case JNZ: if (A != 0) {
                instruction_pointer = operand;
            }
            break;
        case BXC: B ^= C;
            break;
        case OUT: output_buffer.emplace_back(combo_operand_value(operand) % 8);
            break;
        case BDV: B = A / static_cast<long long>(pow(2, combo_operand_value(operand)));
            break;
        case CDV: C = A / static_cast<long long>(pow(2, combo_operand_value(operand)));
            break;
        }
    }

public:
    void readProgram(std::ifstream& input)
    {
        program.clear();
        instruction_pointer = 0;
        output_buffer.clear();

        unsigned int val;
        while (input >> val) {
            input.ignore(); // Eat comma separator
            program.emplace_back(val);
        }
    }

    void executeProgram()
    {
        output_buffer.clear();
        instruction_pointer = 0;
        while (instruction_pointer < program.size())
        {
            cout << '.' << std::flush; // Progress report
            try {
                process_current_instruction();
            }
            catch (std::exception _){
                break;
            }
        }
    }
};

int main()
{
    cout << "Advent of Code 2024: 17\n";
    logic("17-example.txt"s);
    cout << endl;
    logic("17-input.txt"s);
    return 0;
}

static void logic(string fileName)
{
    CPU_s device;

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    input.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    input >> device.A;
    input.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    input >> device.B;
    input.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    input >> device.C;

    input.ignore(std::numeric_limits<std::streamsize>::max(), ':');
    device.readProgram(input);

    // PART 1 - Execute program and show output
    cout << "Processing ";
    device.executeProgram();
    cout << "\nOutput:" << endl
         << std::accumulate(std::next(std::cbegin(device.output_buffer)), std::cend(device.output_buffer),
                            std::to_string(device.output_buffer.front()),
                            [](std::string s, long long val)
                            {
                                return std::move(s) + ',' + std::to_string(val);
                            })
         << endl;
}
