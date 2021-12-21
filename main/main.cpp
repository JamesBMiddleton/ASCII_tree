#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <stdlib.h>


struct Coords {int x, y;};
struct CoordsOffset {int x, y;};

struct BranchInfo
{   
    std::string left_name, right_name;
    CoordsOffset left_offset, right_offset;
};

struct SegmentInfo
// Static class used to create and access segment types and their attributes.
{
    static void create_segment_type(std::string name, std::string ASCII,std::map<std::string, double> probabilities,
                                    std::map<std::string, CoordsOffset> next_segment_pos);
    static void create_branching_segment_type(std::string name, std::string ASCII, BranchInfo branch_info);

    static std::string get_ASCII(std::string name) {return segment_ASCIIs.at(name);}
    static std::map<std::string, double> get_probabilities(std::string name) {return next_segment_probabilities.at(name);}
    static std::map<std::string, CoordsOffset> get_next_segment_coords(std::string name) {return next_segment_positions.at(name);}
    static bool is_branching(std::string name) {return branch_bools.at(name);}
    static BranchInfo get_branch_info(std::string name) {return branch_data.at(name);}
private:
    SegmentInfo() {};

    static std::vector<std::string> segment_names;
    static std::map<std::string, std::string> segment_ASCIIs;
    static std::map<std::string, std::map<std::string, double>> next_segment_probabilities;
    static std::map<std::string, std::map<std::string, CoordsOffset>> next_segment_positions;

    static std::map<std::string, bool> branch_bools;
    static std::map<std::string, BranchInfo> branch_data;
};
std::vector<std::string> SegmentInfo::segment_names;
std::map<std::string, std::string> SegmentInfo::segment_ASCIIs;
std::map<std::string, std::map<std::string, double>> SegmentInfo::next_segment_probabilities;
std::map<std::string, std::map<std::string, CoordsOffset>> SegmentInfo::next_segment_positions;

std::map<std::string, bool> SegmentInfo::branch_bools;
std::map<std::string, BranchInfo> SegmentInfo::branch_data;

void SegmentInfo::create_segment_type(std::string name,std::string ASCII, std::map<std::string, double> probabilities,
                                      std::map<std::string, CoordsOffset> next_segment_pos)
{
    segment_names.push_back(name);
    segment_ASCIIs.insert({name, ASCII});
    branch_bools.insert({name, false});
    next_segment_probabilities.insert({name, probabilities});
    next_segment_positions.insert({name, next_segment_pos});
}

void SegmentInfo::create_branching_segment_type(std::string name, std::string ASCII, BranchInfo branch_info)
{
    segment_names.push_back(name);
    segment_ASCIIs.insert({name, ASCII});
    branch_bools.insert({name, true});
    branch_data.insert({name, branch_info});
}

struct Segment
// Designed for instancing individual segments.
// Access SegmentInfo members using 'type' for a segment's predefined attributes.
{
    Segment(std::string seg_type, Coords seg_coords, bool seg_terminates=false) :type{seg_type}, coords{seg_coords}, is_terminator{seg_terminates} {}
    std::string type;
    Coords coords;
    bool is_terminator;
};

void clear_screen(int x, int y)
// Prints a string of whitespace to clear the screen and removes the cursor.
// This should probably take into account the terminal col number in the future.
{
    int total{x*y};
    std::string s(total, ' ');
    std::cout << s << "\x1b[?25l";
}

std::vector<std::vector<char>> create_canvas(int x, int y)
// Creates a 2D vector of whitespace chars.
{
    std::vector<std::vector<char>> canvas(x, std::vector<char>(y, ' '));
    return canvas;
}

void print_segment(Segment segment)
// Uses the coordinates and type members of a Segment object.
// Accesses SegmentInfo to retrieve the ASCII representation of a segment.
{
    std::cout << "\x1B[" << segment.coords.x << ';' << segment.coords.y << 'H';
    std::cout << SegmentInfo::get_ASCII(segment.type);
    std::cout.flush();   // sleep loops are messed up without flushing cout.
}

void initialize_segment_types()
// SegmentInfo starts empty, each segment type is created here.
{
    SegmentInfo::create_segment_type("trunk_right",
                            "/   /",
                            std::map<std::string, double> {{"trunk_right", 1}},
                            std::map<std::string, CoordsOffset> {{"trunk_right", {-1, 1}}, {"new_arm_left", {-1, 0}}});
    SegmentInfo::create_segment_type("trunk_base",
                                "/     \\",
                                std::map<std::string, double> {{"trunk_right", 1}},
                                std::map<std::string, CoordsOffset> {{"trunk_right", {-1, 1}}});
    SegmentInfo::create_segment_type("arm_left",
                                 "\\ \\",
                                std::map<std::string, double> {{"arm_left", 1}},
                                std::map<std::string, CoordsOffset> {{"arm_left", {-1, -1}}});
    SegmentInfo::create_branching_segment_type("new_arm_left",
                                 "\\    /",
                                 BranchInfo{"arm_left", "trunk_right", {-1,-1}, {-1, 2}});
}

Segment initialise_tree(std::vector<std::vector<char>> canvas)
{
    Segment base{"trunk_base", {24, 30}};
    print_segment(base);
    return base;
}

Segment pick_next_segment(Segment previous_segment, std::vector<std::vector<char>> canvas, int i)
{   
    std::string next_seg_name;  // Static for now.
    if (i==1) next_seg_name = "trunk_right";
    if (i==2) next_seg_name = "new_arm_left";


    CoordsOffset offset{SegmentInfo::get_next_segment_coords(previous_segment.type).at(next_seg_name)};
    Coords next_seg_coords{previous_segment.coords.x + offset.x, previous_segment.coords.y + offset.y};
    Segment next_segment{next_seg_name, next_seg_coords};
    return next_segment;
}

void add_segment(Segment previous_segment, std::vector<std::vector<char>> canvas)
{
    int i{1}; // for testing.
    while (i<3)
    {
        Segment next_segment = pick_next_segment(previous_segment, canvas, i);
        // add_to_canvas(next_segment, canvas)
        print_segment(next_segment);
        if (next_segment.is_terminator)
            break;
        if (SegmentInfo::is_branching(next_segment.type))
        {
            BranchInfo branch_info = SegmentInfo::get_branch_info(next_segment.type);

            Coords left_branch_coords{next_segment.coords.x + branch_info.left_offset.x, next_segment.coords.y + branch_info.left_offset.y};
            std::string left_branch_type{branch_info.left_name};
            Segment left_branch{left_branch_type, left_branch_coords};
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            print_segment(left_branch);
            // add_to_canvas(left_branch);
            // add_segment(left_branch, canvas);  // Recurse.

            Coords right_branch_coords{next_segment.coords.x + branch_info.right_offset.x, next_segment.coords.y + branch_info.right_offset.y};
            std::string right_branch_type{branch_info.right_name};
            Segment right_branch{right_branch_type, right_branch_coords};
            Segment previous_segment = right_branch;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            print_segment(right_branch);
            // add_to_canvas(right_branch);
        }
        else
            previous_segment = next_segment;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        i++;
    }
}

int main()
{
    initialize_segment_types();
    clear_screen(24, 80);
    std::vector<std::vector<char>> canvas = create_canvas(24, 80);
    Segment base = initialise_tree(canvas);
    add_segment(base, canvas);
    //std::cout << "\x1B[" << 24 << ";" << 1 << "H";  // there's something weird happening with cout buffering/flushing.
    return 0;
}
