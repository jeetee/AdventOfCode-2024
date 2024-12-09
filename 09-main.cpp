#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <algorithm>
#include <execution>
#include <list>
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
    cout << "Advent of Code 2024: 09\n";
    logic("09-example.txt"s);
    cout << endl;
    logic("09-example-2.txt"s); // 2132
    cout << endl;
    logic("09-input.txt"s);
    return 0;
}

typedef struct File_s
{
    int ID; // Negative ID == gap
    size_t size;
} File_t;


static void logic(string fileName)
{
    vector<int> disk_blocks; // Block value [0+ == file_ID, <0 == gap
    std::list<File_t> disk;

    cout << "Reading " << fileName << endl;
    std::ifstream input(fileName);
    int file_ID { 0 };
    for (string line; std::getline(input, line);) {
        bool gap = false;
        // Expand
        for (const char& c : line) {
            size_t size = static_cast<size_t>(c - '0');
            if (size > 0) {
                disk.emplace_back(File_t{ gap ? -1 : file_ID, size });
            }
            for (; size --> 0;) {
                disk_blocks.emplace_back(gap? -1 : file_ID);
            }
            if (!gap) {
                ++file_ID;
            }
            gap = !gap;
        }
    }

    // PART 1
    cout << "Fragmenting ";
    for (size_t left = 0, right = disk_blocks.size() - 1; left < right; ++left) {
        if (disk_blocks[left] < 0)
        {
            cout << '.' << std::flush; // Progress report
            while (disk_blocks[right] < 0)
            { // Skip gaps on the end
                --right;
            }
            if (left < right)
            {
                // Swap & erase
                disk_blocks[left] = disk_blocks[right];
                disk_blocks[right] = -1;
            }
        }
    }

    unsigned long long checksum{ 0 };
    for (size_t idx = 0; (idx < disk_blocks.size()) && (disk_blocks[idx] >= 0); ++idx) {
        checksum += idx * disk_blocks[idx];
    }

    // PART 2
    cout << "\nDefragmenting ";
    for (auto right_it = std::prev(std::end(disk)); right_it != std::next(std::begin(disk)); --right_it) {
        if (right_it->ID > 0) {
            cout << '.' << std::flush; // Progress report
            // Find a gap that can fit me; skip first entry, which is always a file
            auto gap = std::find_if(std::execution::par, std::next(std::begin(disk)), right_it, [&right_it](const auto& file) {
                return (file.ID < 0) && (file.size >= right_it->size);
                });
            if (gap != right_it) {
                if (gap->size == right_it->size) {
                    // Exact match, swap file and gap
                    std::iter_swap(gap, right_it);
                }
                else {
                    // Gap is larger than file, we need to reduce it and move file in front of it
                    auto right_ref = std::prev(right_it); // Save ref to last non-changed element
                    gap->size -= right_it->size;
                    disk.splice(gap, disk, right_it);
                    // Restore iterator and insert the hole it left
                    gap = right_it; // Keep temp ref for size of moved element
                    right_it = std::next(right_ref); // Restore ref to last processed element
                    disk.emplace(right_it, File_t{ -gap->ID, gap->size });
                    --right_it; // Skip the just inserted gap
                }
            }
        }
    }
    // Unwind file entries to block level constructs
    unsigned long long checksum_defragmented{ 0 };
    size_t idx{ 0 };
    for (const auto& fileOrGap : disk) {
        if (fileOrGap.ID < 0) {
            idx += fileOrGap.size;
        }
        else {
            for (size_t size = fileOrGap.size; size-- > 0; ++idx) {
                checksum_defragmented += fileOrGap.ID * idx;
            }
        }
    }

    cout << "\nFragmented disk checksum = " << checksum
         << "\nDefragmented disk checksum = " << checksum_defragmented << endl;
}
