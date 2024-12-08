#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <algorithm>
#include <execution>
#include <vector>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using std::cout;
using std::endl;
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
    cout << "Advent of Code 2024: 07\n";
    logic("07-example.txt"s);
    cout << endl;
    logic("07-input.txt"s);
    return 0;
}

static bool evaluate_terms(unsigned long long target, unsigned long long seed, vector<unsigned long long>::const_iterator term, vector<unsigned long long>::const_iterator end);
static bool evaluate_terms_2(unsigned long long target, unsigned long long seed, vector<unsigned long long>::const_iterator term, vector<unsigned long long>::const_iterator end);

static void logic(string fileName)
{
    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);

    unsigned long long calibration_result { 0 };
    unsigned long long calibration_result_2 { 0 };
    for (string line; std::getline(input, line);) {
        cout << "." << std::flush; // Progress report
        std::stringstream ss(line);
        unsigned long long possible_calibration_result;
        ss >> possible_calibration_result; ss.ignore();
        vector<unsigned long long> terms;
        for (unsigned long long term; ss >> term;) {
            terms.emplace_back(term);
        }
        if (evaluate_terms(possible_calibration_result, terms.front(), std::next(std::cbegin(terms)), std::cend(terms)))
        {// Real calibration result
            calibration_result += possible_calibration_result;
            calibration_result_2 += possible_calibration_result;
        }
        else if (evaluate_terms_2(possible_calibration_result, terms.front(), std::next(std::cbegin(terms)), std::cend(terms)))
        {
            calibration_result_2 += possible_calibration_result;
        }
    }

    cout << "\nTotal calibration result = " << calibration_result
         << "\nTotal calibration result with concatenation = " << calibration_result_2 << endl;
}

// PART 1
static bool evaluate_terms(unsigned long long target, unsigned long long seed, vector<unsigned long long>::const_iterator term, vector<unsigned long long>::const_iterator end)
{
    if (term == end)
    {
        return false;
    }

    // Try multiplication first
    unsigned long long temp_term = seed * *term;
    if (temp_term == target)
    {
        return true;
    }
    else if (temp_term < target)
    { // Continue trying
        if (evaluate_terms(target, temp_term, std::next(term), end)) {
            return true;
        }
    }
    // else if > target --> too far, try next option instead
    // Next option = addition
    temp_term = seed + *term;
    if (temp_term == target)
    {
        return true;
    }
    else if (temp_term < target)
    {
        return (evaluate_terms(target, temp_term, std::next(term), end));
    }
    // else (temp_term > target): all other code paths have returned already
    return false;
}

// PART 2 - also include concatenation as operator
static bool evaluate_terms_2(unsigned long long target, unsigned long long seed, vector<unsigned long long>::const_iterator term, vector<unsigned long long>::const_iterator end)
{
    if (term == end)
    {
        return false;
    }

    // Try multiplication first
    unsigned long long temp_term = seed * *term;
    if (temp_term == target)
    {
        return true;
    }
    else if (temp_term < target)
    { // Continue trying
        if (evaluate_terms_2(target, temp_term, std::next(term), end)) {
            return true;
        }
    }
    // else if > target --> too far, try next option instead
    // Next option = concatenation
    std::stringstream concat;
    concat << seed << *term;
    concat >> temp_term;
    if (temp_term == target)
    {
        return true;
    }
    else if (temp_term < target)
    { // Continue trying
        if (evaluate_terms_2(target, temp_term, std::next(term), end)) {
            return true;
        }
    }
    // else if > target --> too far, try next option instead
    // Next option = addition
    temp_term = seed + *term;
    if (temp_term == target)
    {
        return true;
    }
    else if (temp_term < target)
    {
        return (evaluate_terms_2(target, temp_term, std::next(term), end));
    }
    // else (temp_term > target): all other code paths have returned already
    return false;
}
