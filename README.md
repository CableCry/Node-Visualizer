# Node Visualizer

A C++ graph visualizer for simulating **Breadth-First Search (BFS)** and **Depth-First Search (DFS)** traversal algorithms with live animation using [Raylib](https://www.raylib.com/) and [Raygui](https://github.com/raysan5/raygui).


## Features

- 📈 Animated BFS and DFS traversal
- 🎨 Dynamic coloring for node states: Unvisited, Visiting, Visited, and Shortest Path
- 🔁 Toggle between BFS and DFS
- 🔄 Restart animation from the beginning
- 🧩 Node layout is automatically calculated via BFS-based level spacing
- 📁 Input graph is read from a simple `.txt` file


## Requirements

- C++20 compatible compiler (e.g. `g++ 10+`)
- [Raylib](https://github.com/raysan5/raylib)
- [Raygui](https://github.com/raysan5/raygui)
- Make


## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/yourusername/node-visualizer.git
cd node-visualizer
```

### 2. Build the Project
Make sure you have Raylib and Raygui source files in your src or external folder.

```bash
make
```

### 3. Run the Program
```bash
./Node-Visualizer
```

## File Format
The program reads a graph from test.txt (or any .txt you load manually). The format is:

``` txt
1 -> 2, 4
2 -> 3
3 -> 6
4 -> 5, 8
5 ->
6 -> 7, 9
7 ->
8 ->
9 -> 10
10 -> 11
11 -> -1
```
Nodes can point to zero or more nodes (comma-separated).
The start node must have a value of 1 and end node should have a value of -1.
Empty lines or dangling arrows are ignored.


## Credits
Built with:
- [Raylib](https://github.com/raysan5/raylib)
- [Raygui](https://github.com/raysan5/raygui)