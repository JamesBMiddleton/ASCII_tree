#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <stdlib.h>


enum class SegmentTypes {trunk_left, trunk_right, trunk_up};

std::map<SegmentTypes, std::string> segment_ASCII {{SegmentTypes::trunk_left, "\\   \\" }, {SegmentTypes::trunk_right, "/   /"}, {SegmentTypes::trunk_up, "|   |"}};
std::map<SegmentTypes, std::map<SegmentTypes, double>> probabilities;

void clear_screen(int x, int y)
// Prints a string of whitespace to clear the screen.
// This should probably take into account the terminal col number in the future.
{
    int total{x*y};
    std::string s(total, ' ');
    std::cout << s;
}

std::vector<std::vector<char>> create_canvas(int x, int y)
{
    std::vector<std::vector<char>> canvas(x, std::vector<char>(y, ' '));
    return canvas;
}

void print_to_screen(int row, int column, const std::string& segment)
{
    std::cout << "\x1B[" << row << ';' << column << 'H';
    std::cout << segment;
    std::cout.flush();   // sleep loops are messed up without flushing cout.
}

int main()
{
    std::cout << "\x1b[?25l";
    clear_screen(80, 24);
    create_canvas(80, 24);
    int row = 24;
    int col = 1;
    int seg_length = segment_ASCII[SegmentTypes::trunk_right].size();
    for (int i{row}; i>0; --i)
    {
        print_to_screen(i, col, segment_ASCII[SegmentTypes::trunk_right]);
        print_to_screen(i, (row+4)-col, segment_ASCII[SegmentTypes::trunk_left]);
        col++;
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }

    std::cout << "\x1B[" << 24 << ";" << 1 << "H";
    return 0;
}
