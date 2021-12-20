#include <iostream>
#include <vector>
#include <sstream>
#include <chrono>
#include <thread>
#include <stdlib.h>

std::vector<std::vector<char>> create_canvas()
{
    int x{80};
    int y{24};
    std::vector<std::vector<char>> canvas;
    for (int i{0}; i<y; ++i)
    {
        std::vector<char> temp;  
        for (int j{0}; j<x; ++j)
            temp.push_back(' ');
        canvas.push_back(temp);
    }
    return canvas;
}

void draw_canvas(std::vector<std::vector<char>> canvas, int num)
{
    std::stringstream ss;
    ss >> std::noskipws;
    std::string output;
    for (int x{0}; x<canvas.size(); ++x)
    {
        for (int y{0}; y<canvas[0].size(); ++y)
            ss << canvas[x][y];
    }
    output = ss.str();
    output[num] = '*';
    std::cout << output;
    std::cout << "\x1B[0;0H";
}

int main()
{
    std::cout << "\x1b[?25l";
    std::vector<std::vector<char>> canvas = create_canvas();

    double num{1920};
    while (num--)
    {   
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        draw_canvas(canvas, num);
    }
    return 0;
}
