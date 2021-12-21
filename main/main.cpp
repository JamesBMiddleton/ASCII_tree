#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <stdlib.h>


struct Coords {int x, y;};
struct CoordsOffset {int x, y;};
struct BranchCoordsOffsets {CoordsOffset left_branch, right_branch;};
struct BranchNames {std::string left_branch, right_branch;};

struct SegmentInfo
{
    static void create_segment(std::string name, std::string ASCII,std::map<std::string, double> probabilities,
                               std::map<std::string, CoordsOffset> next_segment_pos);
    static void create_branching_segment(std::string name, std::string ASCII, std::map<std::string, double> probabilities, 
                                         BranchCoordsOffsets relative_branch_pos, BranchNames branch_names);

    static std::string get_ASCII(std::string name) {return segment_ASCIIs[name];}
    static std::map<std::string, double> get_probabilities(std::string name) {return next_segment_probabilities[name];}
    static std::map<std::string, CoordsOffset> get_next_segment_coords(std::string name) {return next_segment_positions[name];}
    static bool is_branching(std::string name) {return branch_bools[name];}
    static BranchCoordsOffsets get_relative_branch_coords(std::string name) {return branching_segment_branch_coords[name];}
    static BranchNames get_branch_names(std::string name) {return branching_segment_branch_names[name];}

private:
    SegmentInfo() {};

    static std::vector<std::string> segment_names;
    static std::map<std::string, std::string> segment_ASCIIs;
    static std::map<std::string, std::map<std::string, double>> next_segment_probabilities;
    static std::map<std::string, std::map<std::string, CoordsOffset>> next_segment_positions;
    static std::map<std::string, bool> branch_bools;
    static std::map<std::string, BranchCoordsOffsets> branching_segment_branch_coords;
    static std::map<std::string, BranchNames> branching_segment_branch_names;
};
std::vector<std::string> SegmentInfo::segment_names;
std::map<std::string, std::string> SegmentInfo::segment_ASCIIs;
std::map<std::string, std::map<std::string, double>> SegmentInfo::next_segment_probabilities;
std::map<std::string, std::map<std::string, CoordsOffset>> SegmentInfo::next_segment_positions;
std::map<std::string, bool> SegmentInfo::branch_bools;
std::map<std::string, BranchCoordsOffsets> SegmentInfo::branching_segment_branch_coords;
std::map<std::string, BranchNames> SegmentInfo::branching_segment_branch_names;

void SegmentInfo::create_segment(std::string name,std::string ASCII, std::map<std::string, double> probabilities,
                                 std::map<std::string, CoordsOffset> next_segment_pos)
{
    segment_names.push_back(name);
    segment_ASCIIs.insert({name, ASCII});
    branch_bools.insert({name, false});
    next_segment_probabilities.insert({name, probabilities});
    next_segment_positions.insert({name, next_segment_pos});
}

void SegmentInfo::create_branching_segment(std::string name, std::string ASCII, std::map<std::string, double> probabilities, 
                                           BranchCoordsOffsets relative_branch_pos, BranchNames branch_names)
{
    segment_names.push_back(name);
    segment_ASCIIs.insert({name, ASCII});
    branch_bools.insert({name, true});
    branching_segment_branch_coords.insert({name, relative_branch_pos});
}

struct Segment
{
    Segment(std::string n, Coords c, bool t=false, bool b=false) :type{n}, coords{c}, is_terminator{t}, is_branching{b} {}
    std::string type;
    Coords coords;
    bool is_terminator;
    bool is_branching;
};

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

void print_to_screen(Segment segment)
{
    std::cout << "\x1B[" << segment.coords.x << ';' << segment.coords.y << 'H';
    std::cout << SegmentInfo::get_ASCII(segment.type);
    std::cout.flush();   // sleep loops are messed up without flushing cout.
}

void initialize_segment_types()
{
    SegmentInfo::create_segment("trunk_right",
                            "/   /",
                            std::map<std::string, double> {{"trunk_right", 1}},
                            std::map<std::string, CoordsOffset> {{"trunk_right", {1, 0}}});
    SegmentInfo::create_segment("base",
                                "/     \\",
                                std::map<std::string, double> {{"trunk_right", 1}},
                                std::map<std::string, CoordsOffset> {{"trunk_right", {1, 0}}});
}

Segment initialise_tree(std::vector<std::vector<char>> canvas)
{
    Segment base{"base", {24, 40}};
    return base;
}

int main()
{
    initialize_segment_types();
    clear_screen(24, 80);
    std::vector<std::vector<char>> canvas = create_canvas(24, 80);
    Segment base = initialise_tree(canvas);
    print_to_screen(base);
    //std::cout << "\x1B[" << 24 << ";" << 1 << "H";
    return 0;
}
