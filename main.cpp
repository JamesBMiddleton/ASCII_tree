#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <thread>

struct Coords {int x, y;};
struct CoordsOffset {int x, y;};

struct BranchInfo
{   
    std::string left_name, right_name;
    CoordsOffset left_offset, right_offset;
    bool predefined_next_segs;
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
        std::map<std::string, int> {{"trunk_left", 3}, {"trunk_straight", 2}, {"trunk_right", 1}, {"new_arm_left", 1}, {"new_arm_right", 1}, {"trunk_split", 0}},
        std::map<std::string, CoordsOffset> {{"trunk_left", {-1, -1}}, {"trunk_straight", {-1, 0}}, {"trunk_right", {-1, 0}}, {"new_arm_left", {-1, -1}}, {"new_arm_right", {-1, -1}}, {"trunk_split", {-1,-1}}});
    SegmentInfo::create_segment_type(
        "trunk_straight",
        "|   |",
        std::map<std::string, int> {{"trunk_left", 1}, {"trunk_straight", 3}, {"trunk_right", 1}, {"new_arm_left", 1}, {"new_arm_right", 1}, {"trunk_split", 0}},
        std::map<std::string, CoordsOffset> {{"trunk_left", {-1, 0}}, {"trunk_straight", {-1, 0}}, {"trunk_right", {-1, 0}}, {"new_arm_left", {-1, 0}}, {"new_arm_right", {-1, -1}}, {"trunk_split", {-1,0}}});
    SegmentInfo::create_segment_type(
        "trunk_right",
        "/   /",
        std::map<std::string, int> {{"trunk_left", 1}, {"trunk_straight", 2}, {"trunk_right", 3}, {"new_arm_left", 1}, {"new_arm_right", 1}, {"trunk_split", 0}},
        std::map<std::string, CoordsOffset> {{"trunk_left", {-1, 0}}, {"trunk_straight", {-1, 0}}, {"trunk_right", {-1, 1}}, {"new_arm_left", {-1, 0}}, {"new_arm_right", {-1, 0}}, {"trunk_split", {-1,0}}});


    SegmentInfo::create_branching_segment_type(
        "new_arm_left",
        "\\    /",
        BranchInfo{"arm_left", "trunk_right", {-1,-1}, {-1, 2}, true});
    SegmentInfo::create_branching_segment_type(
        "new_arm_right",
        "\\    /",
        BranchInfo{"trunk_left", "arm_right", {-1,-1}, {-1, 4}, true});
    SegmentInfo::create_branching_segment_type(
        "trunk_split",
        "\\ \\/ /",
        BranchInfo{"arm_left", "arm_right", {-1,-1}, {-1, 4}, true});                      


    SegmentInfo::create_segment_type(
        "arm_left",
        "\\ \\",
        std::map<std::string, int> {{"arm_left", 3}, {"arm_straight", 2}, {"arm_right", 1}, {"arm_new_twig_right", 2}, {"arm_taper_left", 3}},
        std::map<std::string, CoordsOffset> {{"arm_left", {-1, -1}}, {"arm_straight", {-1, 0}}, {"arm_right", {-1, 0}}, {"arm_new_twig_right", {-1, -1}}, {"arm_taper_left", {-1, 0}}});
    SegmentInfo::create_segment_type(
        "arm_straight",
        "| |",
        std::map<std::string, int> {{"arm_left", 2}, {"arm_straight", 1}, {"arm_right", 2}, {"arm_taper_left", 1}, {"arm_taper_right", 3}},
        std::map<std::string, CoordsOffset> {{"arm_left", {-1, 0}}, {"arm_straight", {-1, 0}}, {"arm_right", {-1, 0}}, {"arm_taper_left", {-1, 0}}, {"arm_taper_right", {-1, 1}}});
    SegmentInfo::create_segment_type(
        "arm_right",
        "/ /",
        std::map<std::string, int> {{"arm_left", 1}, {"arm_straight", 2}, {"arm_right", 3}, {"arm_new_twig_left", 2}, {"arm_taper_right", 3}},
        std::map<std::string, CoordsOffset> {{"arm_left", {-1, 0}}, {"arm_straight", {-1, 0}}, {"arm_right", {-1, 1}}, {"arm_new_twig_left", {-1, 0}}, {"arm_taper_right", {-1, 1}}});
    

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
        "arm_new_twig_left",
        "\\/ /",
        BranchInfo{"twig_left", "arm_right", {-1,-1}, {-1, 2}, true});
    SegmentInfo::create_branching_segment_type(
        "arm_new_twig_right",
        "\\ \\/",
        BranchInfo{"arm_left", "twig_right", {-1,-1}, {-1, 1}, true});

    
    SegmentInfo::create_segment_type(
        "twig_left",
        "\\",
        std::map<std::string, int> {{"twig_left", 1}, {"twig_straight", 1}, {"twig_right", 1}, {"twig_across_left", 3}, {"twig_new_twig_right", 2}, {"twig_new_twig_across_left", 3}},
        std::map<std::string, CoordsOffset> {{"twig_left", {-1, -1}}, {"twig_straight", {-1, 0}}, {"twig_right", {-1, 0}}, {"twig_across_left", {-1, -1}}, {"twig_new_twig_right", {-1, -1}}, {"twig_new_twig_across_left", {-1, -1}}});
    SegmentInfo::create_segment_type(
        "twig_straight",
        "|",
        std::map<std::string, int> {{"twig_left", 1}, {"twig_straight", 3}, {"twig_right", 3}},
        std::map<std::string, CoordsOffset> {{"twig_left", {-1, 0}}, {"twig_straight", {-1, 0}}, {"twig_right", {-1, 0}}});
    SegmentInfo::create_segment_type(
        "twig_right",
        "/",
        std::map<std::string, int> {{"twig_left", 1}, {"twig_straight", 1}, {"twig_right", 1}, {"twig_across_right", 3}, {"twig_new_twig_left", 2}, {"twig_new_twig_across_right", 3}},
        std::map<std::string, CoordsOffset> {{"twig_left", {-1, 0}}, {"twig_straight", {-1, 0}}, {"twig_right", {-1, 1}}, {"twig_across_right", {-1, 1}}, {"twig_new_twig_left", {-1, 0}}, {"twig_new_twig_across_right", {-1, 0}}});
    SegmentInfo::create_segment_type(
        "twig_across_left",
        "_",
        std::map<std::string, int> {{"twig_left", 2}, {"twig_across_left", 1}},
        std::map<std::string, CoordsOffset> {{"twig_left", {0, -1}}, {"twig_across_left", {0, -1}}});
    SegmentInfo::create_segment_type(
        "twig_across_right",
        "_",
        std::map<std::string, int> {{"twig_right", 2}, {"twig_across_right", 1}},
        std::map<std::string, CoordsOffset> {{"twig_right", {0, 1}}, {"twig_across_right", {0, 1}}});


    SegmentInfo::create_branching_segment_type(
        "twig_new_twig_left",
        "\\/",
        BranchInfo{"twig_left", "twig_right", {0,0}, {0,1}, false});
    SegmentInfo::create_branching_segment_type(
        "twig_new_twig_right",
        "\\/",
        BranchInfo{"twig_left", "twig_right", {0,0}, {0,1}, false});
    SegmentInfo::create_branching_segment_type(
        "twig_new_twig_across_left",
        "_/",
        BranchInfo{"twig_across_left", "twig_right", {0,0}, {0,1}, false});
    SegmentInfo::create_branching_segment_type(
        "twig_new_twig_across_right",
        "\\_",
        BranchInfo{"twig_left", "twig_across_right", {0,0}, {0,1}, false});
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
// Keeps track of where segments have been placed on the screen.
{
    Canvas(long unsigned int x, long unsigned int y);
    std::vector<std::vector<char>> ASCII_canvas;
    std::string force_segment_split(Segment& segment);
    bool terminates(Segment& segment);
    unsigned long segment_count{0};
    bool has_hollow{false};
private:
    static int max_trunk_height;
    static int max_arm_height;
    static int max_twig_height;
};
int Canvas::max_trunk_height{11};   
int Canvas::max_arm_height{7};          // It's backwards because we work from bottom to top.
int Canvas::max_twig_height{4};

Canvas::Canvas(long unsigned int x, long unsigned int y)
    :ASCII_canvas{x, std::vector<char>(y, '0')}  // Creates a 2D vector of '0' chars.
{}

std::string Canvas::force_segment_split(Segment& segment)
// Checks whether a Segment should split and returns the relevant Segment type to do so.
{
    if ((segment.type == "trunk_left" || segment.type == "trunk_straight" || segment.type == "trunk_right") && segment.coords.x <= max_trunk_height)
        return "trunk_split";
    if (segment.coords.x <= max_arm_height)
    {
        if (segment.type == "arm_left")
            return "arm_taper_left";
        if (segment.type == "arm_straight")
            return "arm_taper_right";  // need randomness here ideally... (left or right)
        if (segment.type == "arm_right")
            return "arm_taper_right";
        else
            return "";
    }
    return "";
}

bool Canvas::terminates(Segment& segment)
// Decides whether a segment should terminate.
{   
    if (segment.coords.x <= max_twig_height)
        return true;
    if (SegmentInfo::get_ASCII(segment.type).size() <= 2)
        return !(rand() % 2); // 50/50 terminate/continue.
    return false;
}

void clear_screen(int x, int y)
// Prints a string of whitespace to clear the screen and removes the cursor.
{
    int total{x*y};
    std::string s(total, ' ');
    std::cout << s << "\x1b[?25l";
    std::cout.flush();
}

std::string add_decor(Segment& segment, Canvas& canvas)
// Adds decoration to a segment's ASCII in random positions.
// A bit of an 'if' mess.
{
    std::string ascii{SegmentInfo::get_ASCII(segment.type)};
    if (ascii.size() >= 5) //trunks
    {  
        if (segment.type == "trunk_split")
        {
            ascii[1] = '~';
            ascii[4] = '~';
        }
        else if (!(rand() % 4) && segment.coords.x < 20 && !canvas.has_hollow)
        {
            ascii[2] = '@';
            canvas.has_hollow = true;
        }
        else
        { 
            int i{rand() % 3 + 1};
            ascii[i] = '~';
        }
    }
    if (ascii.size() == 3) //arms
        ascii[1] = '~';
    if (segment.type == "arm_new_twig_left")
        ascii[2] = '~';
    if (segment.type == "arm_new_twig_right")
        ascii[1] = '~';
    return "\x1b[38;2;161;61;45m" + ascii + "\x1b[m";
}

void add_leaves(Segment& segment, Canvas& canvas)
// prints a circle of leaves around terminating twigs.
{
    static std::vector<Coords> leaf_coords {
        {0,-1}, {0,1}, {-1,0}, {1,0}, {1,1}, {-1,-1}, {1,-1}, {-1,1}, {1,2}, {-2, 1}, {-1, -2}, {-1, 2},
        {1, -2}, {-1, -3}, {-1, 3}, {0, -2}, {0, 2}, {-2, 0}, {-2, 2}, {0, -3},{-2, -1}, {-2, -2}, {0, 3}};
    for (Coords leaf_coord : leaf_coords)
    {
        if (canvas.ASCII_canvas[segment.coords.x + leaf_coord.x][segment.coords.y + leaf_coord.y] == '0')
        {
            std::cout << "\x1B[" << segment.coords.x + leaf_coord.x << ';' << segment.coords.y + leaf_coord.y << 'H';
            std::cout << "\x1b[38;2;45;90;39m" << '&' << "\x1b[m";
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}

void print_segment(Segment& segment, Canvas& canvas)
// Uses the coordinates and type members of a Segment object.
// Accesses SegmentInfo to retrieve the ASCII representation of a segment.
{
    std::string ascii_output{add_decor(segment, canvas)};
    std::cout << "\x1B[" << segment.coords.x << ';' << segment.coords.y << 'H';
    std::cout << ascii_output;
    std::cout.flush();
    if (segment.is_terminator)
        add_leaves(segment, canvas);
}

std::string choose_segment_type(Segment& previous_segment, Canvas& canvas)
// Adds the potential segments to a vector the number of times specified.
// Randomly chooses a winning segment.
// This isn't particularly efficient, but its analogous to a raffle, so easy to understand...
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
    long unsigned int winner{rand() % raffle.size()};
    return raffle[winner];
}

Segment pick_next_segment(Segment& previous_segment, Canvas& canvas)
// Deals with creating the next Segment object to pass through the add_segment() loop.
// Performs checks to make sure the tree will not grow out of frame.
{   
    std::string next_seg_name;
    std::string force_split = canvas.force_segment_split(previous_segment);
    if (force_split != "") // hacky way to check if we want to force a split...
        next_seg_name = force_split;
    else
        next_seg_name = choose_segment_type(previous_segment, canvas);
    
    CoordsOffset offset{SegmentInfo::get_next_segment_coords(previous_segment.type).at(next_seg_name)};
    Coords next_seg_coords{previous_segment.coords.x + offset.x, previous_segment.coords.y + offset.y};
    Segment next_segment{next_seg_name, next_seg_coords, canvas.terminates(previous_segment)};
    canvas.segment_count++;
    return next_segment;
}

void add_to_canvas(Segment& segment, Canvas& canvas)
// Adds segment to a 2D vector the size of the terminal window (currently only supporting default 80x24).
{
    int row{segment.coords.x}; // backwards...
    std::string segment_ASCII = SegmentInfo::get_ASCII(segment.type);
    for (int i{0}; i<segment_ASCII.size(); i++)
        canvas.ASCII_canvas[row][segment.coords.y+i] = segment_ASCII[i];
}

void add_segment(Segment& previous_segment, Canvas& canvas)
// Main program loop. Recurses when a Segment creates a new branch.
// 'While' loop breaks when a terminator segment is reached.
{
    while (true)
    {
        Segment next_segment = pick_next_segment(previous_segment, canvas);
        add_to_canvas(next_segment, canvas);
        print_segment(next_segment, canvas);
        if (next_segment.is_terminator)
            break;
        if (SegmentInfo::is_branching(next_segment.type))
        {
            BranchInfo branch_info = SegmentInfo::get_branch_info(next_segment.type);

            Coords left_branch_coords{next_segment.coords.x + branch_info.left_offset.x, next_segment.coords.y + branch_info.left_offset.y};
            std::string left_branch_type{branch_info.left_name};
            Segment left_branch{left_branch_type, left_branch_coords};
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            if (branch_info.predefined_next_segs)
            {
                add_to_canvas(left_branch, canvas);
                print_segment(left_branch, canvas);
            }
            add_segment(left_branch, canvas);  // Recurse.

            Coords right_branch_coords{next_segment.coords.x + branch_info.right_offset.x, next_segment.coords.y + branch_info.right_offset.y};
            std::string right_branch_type{branch_info.right_name};
            Segment right_branch{right_branch_type, right_branch_coords};
            previous_segment = right_branch;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            if (branch_info.predefined_next_segs) 
            {   
                add_to_canvas(right_branch, canvas);
                print_segment(right_branch, canvas);
            }
        }
        else
            previous_segment = next_segment;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

Segment initialise_tree(Canvas& canvas)
// Places the first Segment on the screen and canvas.
// Returns the Segment object required to start the main recursion loop 'add_segment()'.
{
    Segment base{"trunk_base", {23, 32}};
    canvas.segment_count++;
    print_segment(base, canvas);
    return base;
}

int main(int argc, char* argv[])
{
    if (argc > 1 && std::string(argv[1]) == "--seed")
        srand(std::stoi(argv[2]));
    else
        srand(time(NULL)); 
    initialize_segment_types();
    clear_screen(24, 80);
    Canvas canvas{24, 80};
    Segment base = initialise_tree(canvas);
    add_segment(base, canvas);
    std::cout << "\x1B[" << 24 << ";" << 1 << "H" << "\x1b[?25h";  // returns cursor to bottom of terminal.
    return 0;
}
