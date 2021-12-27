## ASCII_tree 

<img src="https://user-images.githubusercontent.com/73485794/147416532-d28aeccf-8a9d-417c-98c3-7184be14f05b.gif" align="right">
<br>
<br>
<br>
<br>
<br>
<br>

ASCII_tree uses procedural generation to produce coloured tree-like structures in a terminal window using ASCII characters.
Inspired by [cbonsai](https://gitlab.com/jallbrit/cbonsai).

<br>
<br>
<br>
<br>
<br>
<br>

### Requirements

Must be run in a terminal emulator that supports *ANSI escape codes* and *24 bit colour*. <br>

_Supported_ - Konsole (KDE), Xterm, Iterm (MacOS), libvte (GNOME, Xfce).<br>
_Unsupported_ - Windows CMD, Powershell (unless 24 bit colour is manually enabled).

### Installation & Usage
ASCII_tree has no dependencies outside the C++ standard library, just compile and run! <br>
Add the argument "--seed 123" to specify a random seed.

### How it works
The main data structures of the program include a purely static class _SegmentInfo_, which holds information about each segment type, such as which other segment types can follow a given segment type. For example "trunk_left" is a segment type represented by the ASCII sequence "\  ~ \\", which can be followed by "trunk_straight", "trunk_right" etc... The class _Segment_ is used to create instances which have a segment type and a set of coordinates for where it should be printed on the screen. The class _Canvas_ keeps track of where segments have already been printed on the screen. <br>
The main program loop occurs within the function _add_segment()_. The _Segment_ instance passed to _add_segment()_ is used to create a new _Segment_ instance with a valid segment type and coordinates that are offset correctly from the previous _Segment_ instance. This process loops within _add_segment()_ until reaching a branching segment type, which results in _add_segment()_ being called (recursively). The loop is broken when a terminating _Segment_ instance is reached.

### Issues & Improvements

The main problem with this current version is that branches have no knowledge of where other branches are, meaning on occasions where multiple branches overlap, the result can end up looking like a jumbled mess of ASCII characters not particularly representative of how trees grow.<br> On a similar note, an issue I ran into was the tradeoff between constraining the tree growth to produce recognisably "tree-like" structures and allowing more variety in the growth of the trees. In its current state it is tuned more towards producing recognisable trees (which usually look similar to acacia trees) with every random seed. To add more variety while still mainly producing recognisably "tree-like" structures, a solution would be to create sets of constraints which could be chosen to produce different types of trees.
