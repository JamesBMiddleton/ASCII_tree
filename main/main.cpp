#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <time.h>
#include <random>


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
    static void create_segment_type(std::string name, std::string ASCII,std::map<std::string, int> probabilities,
                                    std::map<std::string, CoordsOffset> next_segment_pos);
    static void create_branching_segment_type(std::string name, std::string ASCII, BranchInfo branch_info);

    static std::string get_ASCII(std::string name) {return segment_ASCIIs.at(name);}
    static std::map<std::string, int> get_probabilities(std::string name) {return next_segment_probabilities.at(name);}
    static std::map<std::string, CoordsOffset> get_next_segment_coords(std::string name) {return next_segment_positions.at(name);}
    static bool is_branching(std::string name) {return branch_bools.at(name);}
    static BranchInfo get_branch_info(std::string name) {return branch_data.at(name);}
private:
    SegmentInfo() {};

    static std::vector<std::string> segment_names;
    static std::map<std::string, std::string> segment_ASCIIs;
    static std::map<std::string, std::map<std::string, int>> next_segment_probabilities;
    static std::map<std::string, std::map<std::string, CoordsOffset>> next_segment_positions;

    static std::map<std::string, bool> branch_bools;
    static std::map<std::string, BranchInfo> branch_data;
};
std::vector<std::string> SegmentInfo::segment_names;
std::map<std::string, std::string> SegmentInfo::segment_ASCIIs;
std::map<std::string, std::map<std::string, int>> SegmentInfo::next_segment_probabilities;
std::map<std::string, std::map<std::string, CoordsOffset>> SegmentInfo::next_segment_positions;

std::map<std::string, bool> SegmentInfo::branch_bools;
std::map<std::string, BranchInfo> SegmentInfo::branch_data;

void SegmentInfo::create_segment_type(std::string name,std::string ASCII, std::map<std::string, int> probabilities,
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
    Segment(std::string seg_type, Coords seg_coords, bool seg_terminates=false) 
            :type{seg_type}, coords{seg_coords}, is_terminator{seg_terminates} {}
    std::string type;
    Coords coords;
    bool is_terminator;
};

struct Canvas
{
    Canvas(int x, int y);
    std::vector<std::vector<char>> ASCII_canvas;
    unsigned long get_seed() {return seed+segment_count;}
    unsigned long segment_count;
private:
    long seed;
};

Canvas::Canvas(int x, int y)
    :ASCII_canvas{(x, std::vector<char>(y, ' '))}, segment_count{0}, seed{time(NULL)}   // Creates a 2D vector of whitespace chars.
{}

void clear_screen(int x, int y)
// Prints a string of whitespace to clear the screen and removes the cursor.
// This should probably take into account the terminal col number in the future.
{
    int total{x*y};
    std::string s(total, ' ');
    std::cout << s << "\x1b[?25l";
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
    SegmentInfo::create_segment_type(
        "trunk_base",
        "/     \\",
        std::map<std::string, int> {{"trunk_left", 1}, {"trunk_straight", 3}, {"trunk_right", 1}},
        std::map<std::string, CoordsOffset> {{"trunk_left", {-1, 1}}, {"trunk_straight", {-1, 1}}, {"trunk_right", {-1, 1}}});
    SegmentInfo::create_segment_type(
        "trunk_left",
        "\\   \\",
        std::map<std::string, int> {{"trunk_left", 2}, {"trunk_straight", 5}, {"trunk_right", 1}, {"new_arm_left", 1}, {"new_arm_right", 1}, {"trunk_split", 1}},
        std::map<std::string, CoordsOffset> {{"trunk_left", {-1, -1}}, {"trunk_straight", {-1, 0}}, {"trunk_right", {-1, 0}}, {"new_arm_left", {-1, -1}}, {"new_arm_right", {-1, -1}}, {"trunk_split", {-1,-1}}});
    SegmentInfo::create_segment_type(
        "trunk_straight",
        "|   |",
        std::map<std::string, int> {{"trunk_left", 1}, {"trunk_straight", 5}, {"trunk_right", 1}, {"new_arm_left", 1}, {"new_arm_right", 1}, {"trunk_split", 1}},
        std::map<std::string, CoordsOffset> {{"trunk_left", {-1, 0}}, {"trunk_straight", {-1, 0}}, {"trunk_right", {-1, 0}}, {"new_arm_left", {-1, 0}}, {"new_arm_right", {-1, -1}}, {"trunk_split", {-1,0}}});
    SegmentInfo::create_segment_type(
        "trunk_right",
        "/   /",
        std::map<std::string, int> {{"trunk_left", 1}, {"trunk_straight", 5}, {"trunk_right", 2}, {"new_arm_left", 1}, {"new_arm_right", 1}, {"trunk_split", 1}},
        std::map<std::string, CoordsOffset> {{"trunk_left", {-1, 0}}, {"trunk_straight", {-1, 0}}, {"trunk_right", {-1, 1}}, {"new_arm_left", {-1, 0}}, {"new_arm_right", {-1, 0}}, {"trunk_split", {-1,0}}});


    SegmentInfo::create_branching_segment_type(
        "new_arm_left",
        "\\    /",
        BranchInfo{"arm_left", "trunk_right", {-1,-1}, {-1, 2}});
    SegmentInfo::create_branching_segment_type(
        "new_arm_right",
        "\\    /",
        BranchInfo{"trunk_left", "arm_right", {-1,-1}, {-1, 4}});
    SegmentInfo::create_branching_segment_type(
        "trunk_split",
        "\\ \\/ /",
        BranchInfo{"arm_left", "arm_right", {-1,-1}, {-1, 4}});                      


    SegmentInfo::create_segment_type(
        "arm_left",
        "\\ \\",
        std::map<std::string, int> {{"arm_left", 2}, {"arm_straight", 2}, {"arm_right", 1}, {"new_twig_right", 1}, {"arm_taper_left", 1}},
        std::map<std::string, CoordsOffset> {{"arm_left", {-1, -1}}, {"arm_straight", {-1, 0}}, {"arm_right", {-1, 0}}, {"new_twig_right", {-1, -1}}, {"arm_taper_left", {-1, 0}}});
    SegmentInfo::create_segment_type(
        "arm_straight",
        "| |",
        std::map<std::string, int> {{"arm_left", 1}, {"arm_straight", 2}, {"arm_right", 1}, {"arm_taper_left", 1}, {"arm_taper_right", 1}},
        std::map<std::string, CoordsOffset> {{"arm_left", {-1, 0}}, {"arm_straight", {-1, 0}}, {"arm_right", {-1, 0}}, {"arm_taper_left", {-1, 0}}, {"arm_taper_right", {-1, 1}}});
    SegmentInfo::create_segment_type(
        "arm_right",
        "/ /",
        std::map<std::string, int> {{"arm_left", 1}, {"arm_straight", 2}, {"arm_right", 2}, {"new_twig_left", 1}, {"arm_taper_right", 1}},
        std::map<std::string, CoordsOffset> {{"arm_left", {-1, 0}}, {"arm_straight", {-1, 0}}, {"arm_right", {-1, 1}}, {"new_twig_left", {-1, 0}}, {"arm_taper_right", {-1, 1}}});
    

    SegmentInfo::create_segment_type(
        "arm_taper_left",
        "|\\",
        std::map<std::string, int> {{"twig_left", 1}},
        std::map<std::string, CoordsOffset> {{"twig_left", {-1, 0}}});
    SegmentInfo::create_segment_type(
        "arm_taper_right",
        "/|",
        std::map<std::string, int> {{"twig_right", 1}},
        std::map<std::string, CoordsOffset> {{"twig_right", {-1, 1}}});
    

    SegmentInfo::create_branching_segment_type(
        "new_twig_left",
        "\\/ /",
        BranchInfo{"twig_left", "arm_right", {-1,-1}, {-1, 2}});
    SegmentInfo::create_branching_segment_type(
        "new_twig_right",
        "\\ \\/",
        BranchInfo{"arm_left", "twig_right", {-1,-1}, {-1, 1}});

    
    SegmentInfo::create_segment_type(
        "twig_left",
        "\\",
        std::map<std::string, int> {{"twig_left", 1}, {"twig_straight", 1}, {"twig_right", 1}},
        std::map<std::string, CoordsOffset> {{"twig_left", {-1, -1}}, {"twig_straight", {-1, 0}}, {"twig_right", {-1, 0}}});
    SegmentInfo::create_segment_type(
        "twig_straight",
        "|",
        std::map<std::string, int> {{"twig_left", 1}, {"twig_straight", 1}, {"twig_right", 1}},
        std::map<std::string, CoordsOffset> {{"twig_left", {-1, 0}}, {"twig_straight", {-1, 0}}, {"twig_right", {-1, 0}}});
    SegmentInfo::create_segment_type(
        "twig_right",
        "/",
        std::map<std::string, int> {{"twig_left", 1}, {"twig_straight", 1}, {"twig_right", 1}},
        std::map<std::string, CoordsOffset> {{"twig_left", {-1, 0}}, {"twig_straight", {-1, 0}}, {"twig_right", {-1, 1}}});
}

Segment initialise_tree(Canvas canvas)
{
    Segment base{"trunk_base", {24, 30}};
    canvas.segment_count++;
    print_segment(base);
    return base;
}

std::string choose_segment_type(Segment previous_segment, Canvas& canvas)
// Adds the potential segments to a vector the number of times specified.
// Randomly chooses a winning segment.
// This definitely isn't efficient, but its analogous to a raffle, so easy to understand...
// rand() % raffle.size() would probably work fine to simplify things.
{
    std::map<std::string, int> potential_segments{SegmentInfo::get_probabilities(previous_segment.type)};
    std::vector<std::string> raffle;
    std::map<std::string, int>::iterator it;
    int value;
    for (it = potential_segments.begin(); it != potential_segments.end(); it++)
    {
        value = it->second;
        while (value--)
            raffle.push_back(it->first);
    }
    srand(canvas.get_seed());                                  
    long unsigned int winner{rand() % raffle.size()};
    canvas.segment_count++;
    return raffle[winner];
}

Segment pick_next_segment(Segment previous_segment, Canvas& canvas)
{   
    std::string next_seg_name = choose_segment_type(previous_segment, canvas);
    CoordsOffset offset{SegmentInfo::get_next_segment_coords(previous_segment.type).at(next_seg_name)};
    Coords next_seg_coords{previous_segment.coords.x + offset.x, previous_segment.coords.y + offset.y};
    Segment next_segment{next_seg_name, next_seg_coords};
    return next_segment;
}

void add_segment(Segment previous_segment, Canvas& canvas)
{
    int i{7};
    while (i--)
    {
        Segment next_segment = pick_next_segment(previous_segment, canvas);
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
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            print_segment(left_branch);
            // add_to_canvas(left_branch);
            add_segment(left_branch, canvas);  // Recurse. *note this makes the tree grow left to right, need multithreading or randomness here.* also left branch gets extra 7 steps in current testing

            Coords right_branch_coords{next_segment.coords.x + branch_info.right_offset.x, next_segment.coords.y + branch_info.right_offset.y};
            std::string right_branch_type{branch_info.right_name};
            Segment right_branch{right_branch_type, right_branch_coords};
            previous_segment = right_branch;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            print_segment(right_branch);
            // add_to_canvas(right_branch);
        }
        else
            previous_segment = next_segment;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

int main()
{
    initialize_segment_types();
    clear_screen(24, 80);
    Canvas canvas{24, 80};
    Segment base = initialise_tree(canvas);
    add_segment(base, canvas);
    //std::cout << "\x1B[" << 24 << ";" << 1 << "H";  // there's something weird happening with cout buffering/flushing.
    return 0;
}
