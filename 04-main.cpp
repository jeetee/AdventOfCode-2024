#include <fstream>
#include <iostream>
#include <string>

#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::string_literals::operator""s;
using std::vector;

static void logic(string fileName);

int main()
{
    cout << "Advent of Code 2024: 04\n";
    logic("04-example.txt"s);
    cout << endl;
    logic("04-input.txt"s);
    return 0;
}

/**
 * @brief find_tail_in_grid
 * @remark NO bounds checking!
 */
static bool find_word_in_grid(const string& word, const vector<string>& grid, size_t x, size_t y, int dir_x, int dir_y);

static void logic(string fileName)
{
    vector<string> lines;

    cout << "Reading " << fileName << " file " << endl;
    std::ifstream input(fileName);
    for (string line; std::getline(input, line);) {
        cout << "." << std::flush; // Progress report
        lines.emplace_back(line);
    }

    // PART 1
    const char first_letter = 'X';
    const string tail_to_match = "MAS";
    unsigned long n_matches = 0;
    const size_t height = lines.size();
    const size_t width = lines.front().size();
    // Scan each possible starting position
    for (size_t y = height; y --> 0;)
    {
        for (size_t x = width; x --> 0;)
        {
            if (lines[y][x] != first_letter)
            {
                continue;
            }
            // First letter matches; try to find the others
            const bool can_look_left = (x >= tail_to_match.size());
            const bool can_look_right = ((x + tail_to_match.size()) < width);
            const bool can_look_up = (y >= tail_to_match.size());
            const bool can_look_down = ((y + tail_to_match.size()) < height);
            if (can_look_left)
            {
                if (find_word_in_grid(tail_to_match, lines, x-1, y, -1, 0))
                {
                    ++n_matches;
                }
                if (can_look_up && find_word_in_grid(tail_to_match, lines, x-1, y-1, -1, -1))
                {
                    ++n_matches;
                }
                if (can_look_down && find_word_in_grid(tail_to_match, lines, x-1, y+1, -1, 1))
                {
                    ++n_matches;
                }
            }
            if (can_look_right)
            {
                if (find_word_in_grid(tail_to_match, lines, x+1, y, 1, 0))
                {
                    ++n_matches;
                }
                if (can_look_up && find_word_in_grid(tail_to_match, lines, x+1, y-1, 1, -1))
                {
                    ++n_matches;
                }
                if (can_look_down && find_word_in_grid(tail_to_match, lines, x+1, y+1, 1, 1))
                {
                    ++n_matches;
                }
            }
            if (can_look_up && find_word_in_grid(tail_to_match, lines, x, y-1, 0, -1))
            {
                ++n_matches;
            }
            if (can_look_down && find_word_in_grid(tail_to_match, lines, x, y+1, 0, 1))
            {
                ++n_matches;
            }
        }
    }
    cout << "\nFound the word '" << first_letter << tail_to_match << "' " << n_matches << " times." << std::flush;

    // PART 2
    unsigned long n_crosses = 0;
    const string match = "MAS";
    const string reverse = "SAM";
    // Scan each position, but only look down and right to prevent double counting/finding a cross
    for (size_t y = height - match.size() + 1; y --> 0;)
    {
        for (size_t x = width - match.size() + 1; x --> 0;)
        {
            if (   (   find_word_in_grid(match, lines, x, y, +1, +1)
                    || find_word_in_grid(reverse, lines, x, y, +1, +1)
                    )
                && (   find_word_in_grid(match, lines, x, y + match.size() - 1, +1, -1)
                    || find_word_in_grid(reverse, lines, x, y + match.size() - 1, +1, -1)
                    )
                )
            {
                ++n_crosses;
            }
        }
    }
    cout << "\nFound " << n_crosses << " crosses." << endl;
}

static bool find_word_in_grid(const string& word, const vector<string>& grid, size_t x, size_t y, int dir_x, int dir_y)
{
    for (const char c : word) {
        if (grid[y][x] != c)
        {
            return false;
        }
        x += dir_x;
        y += dir_y;
    }
    return true;
}

