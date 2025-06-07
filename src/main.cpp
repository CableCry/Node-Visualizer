#include "../raylib/src/raylib.h"
#include "../raylib/src/raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "../raygui/src/raygui.h"

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <sstream>
#include <memory>
#include <fstream>
#include <cctype>
#include <locale>
#include <iostream>
#include <queue>
#include <stack>

#include "node.hpp"

std::unordered_map<std::string, std::unique_ptr<Node>> node_reader(const std::string& filename);
std::string trim(const std::string& str);
void print_graph(const std::unordered_map<std::string, std::unique_ptr<Node>>& nodes);
void draw_node(Node& node);
void assignPositions(std::unordered_map<std::string, std::unique_ptr<Node>>& nodes, const std::string& rootId);
void buildBFSSteps(Node* start, std::vector<Node*>& stepsOut, std::unordered_map<Node*, Node*>& parentMap);
void buildDFSSteps(Node* start, std::vector<Node*>& stepsOut, std::unordered_map<Node*, Node*>& parentMap);
void runSearch(std::unordered_map<std::string, std::unique_ptr<Node>>& nodes, 
               std::vector<Node*>& steps, 
               std::unordered_map<Node*, Node*>& parentMap, 
               bool useDFS);


int main() 
{    
    constexpr int screenWidth {1000};
    constexpr int screenHeight {1000};
    bool useDFS = false;
    bool restartRequested = false;
    Rectangle dfsToggleButton = { 800, 50, 150, 30 };
    Rectangle restartButton = { 800, 100, 150, 30 };

    InitWindow(screenWidth, screenHeight, "Node-Traversal");
    SetTargetFPS(60);
    
    std::unordered_map<std::string, std::unique_ptr<Node>> nodes = node_reader("test.txt");
    assignPositions(nodes, "1");

    std::vector<Node*> steps;
    std::unordered_map<Node*, Node*> parentMap;
    runSearch(nodes, steps, parentMap, useDFS);

    unsigned int stepIndex = 0;
    float stepTimer = 0.0f;
    float stepDelay = 0.2f;

    bool pathTraced = false;

    while (!WindowShouldClose()) {
        stepTimer += GetFrameTime();
        if (stepTimer >= stepDelay && stepIndex < steps.size()) {
            Node* node = steps[stepIndex];
            node->setState(State::VISITING);

            if (stepIndex > 0)
                steps[stepIndex - 1]->setState(State::VISITED);

            stepIndex++;
            stepTimer = 0;
        }

        if (!pathTraced && stepIndex == steps.size()) {
            for (auto& [id, node] : nodes) {
                if (id == "-1") {
                    Node* current = node.get();
                    while (current != nullptr) {
                        current->setState(State::PATH);
                        current = parentMap[current];
                    }
                    pathTraced = true;
                    break;
                }
            }
        }
        if (restartRequested) {
            runSearch(nodes, steps, parentMap, useDFS);
            stepIndex = 0;
            stepTimer = 0;
            pathTraced = false;
            restartRequested = false;
        }
        BeginDrawing();
        ClearBackground(WHITE);

        if (GuiButton(dfsToggleButton, useDFS ? "Mode: DFS" : "Mode: BFS")) {
        useDFS = !useDFS;
        restartRequested = true;
        }

        if (GuiButton(restartButton, "Restart Search")) {
            restartRequested = true;
        }
        for (auto& [id, node] : nodes) {
            draw_node(*node);
        }
        EndDrawing();
    }
    CloseWindow();
}

// Read in the file and insert info into the nodes map
std::unordered_map<std::string, std::unique_ptr<Node>> node_reader(const std::string& filename)
{
    std::ifstream file(filename);
    std::string line;

    std::unordered_map<std::string, std::unique_ptr<Node>> nodes;
    std::unordered_map<std::string, std::vector<std::string>> adjacency;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string from, arrow, toList;

        std::getline(ss, from, '-');
        std::getline(ss, arrow, '>');
        std::getline(ss, toList);

        from = trim(from);
        toList = trim(toList);

        if (!nodes.contains(from))
            nodes[from] = std::make_unique<Node>(from);

        if (toList.empty() || toList == "nothing")
            continue;

        std::stringstream toStream(toList);
        std::string to;
        while (std::getline(toStream, to, ',')) {
            to = trim(to);

            adjacency[from].push_back(to);

            if (!nodes.contains(to))
                nodes[to] = std::make_unique<Node>(to);
        }
    }

    for (auto& [from, targets] : adjacency) {
        for (const auto& to : targets) {
            nodes[from]->addNext(nodes[to].get());
        }
    }

    return nodes;
}

// Basic white space trim from front and back
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    size_t last = str.find_last_not_of(" \t\r\n");
    return (first == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

// Debug function to print the graph
void print_graph(const std::unordered_map<std::string, std::unique_ptr<Node>>& nodes) {
    for (const auto& [id, node] : nodes) {
        std::cout << "Node " << id << " -> ";
        const auto& neighbors = node->getNextNodes();
        if (neighbors.empty()) {
            std::cout << "nothing";
        } else {
            for (const auto* neighbor : neighbors) {
                std::cout << neighbor->getId() << " ";
            }
        }
        std::cout << "\n";
    }
}

// Use bfs to organize the nodes and give them placement
void assignPositions(std::unordered_map<std::string, std::unique_ptr<Node>>& nodes, const std::string& rootId) {
    std::unordered_map<std::string, bool> visited;
    std::queue<std::pair<Node*, int>> queue;

    int spacingX = 75.0f;
    int spacingY = 75.0f;

    std::unordered_map<int, int> levelCount; 

    queue.push({ nodes[rootId].get(), 0 });
    visited[rootId] = true;

    while (!queue.empty()) {
        auto [node, level] = queue.front();
        queue.pop();

        int col = levelCount[level]++;
        node->setPosition({ 50.0f + col * spacingX, 50.0f + level * spacingY });

        for (Node* neighbor : node->getNextNodes()) {
            std::string id = neighbor->getId();
            if (!visited[id]) {
                visited[id] = true;
                queue.push({ neighbor, level + 1 });
            }
        }
    }
}


// Search algos
void runSearch(std::unordered_map<std::string, std::unique_ptr<Node>>& nodes, 
               std::vector<Node*>& steps, 
               std::unordered_map<Node*, Node*>& parentMap, 
               bool useDFS) {
    steps.clear();
    parentMap.clear();

    for (auto& [_, node] : nodes) {
        node->setState(State::UNVISITED);
    }

    if (useDFS)
        buildDFSSteps(nodes["1"].get(), steps, parentMap);
    else
        buildBFSSteps(nodes["1"].get(), steps, parentMap);
}

void buildBFSSteps(Node* start, std::vector<Node*>& stepsOut, std::unordered_map<Node*, Node*>& parentMap) {
    std::unordered_set<Node*> visited;
    std::queue<Node*> q;
    q.push(start);
    visited.insert(start);
    parentMap[start] = nullptr;

    while (!q.empty()) {
        Node* current = q.front(); q.pop();
        stepsOut.push_back(current);

        for (Node* neighbor : current->getNextNodes()) {
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                q.push(neighbor);
                parentMap[neighbor] = current;
            }
        }
    }
}

void buildDFSSteps(Node* start, std::vector<Node*>& stepsOut, std::unordered_map<Node*, Node*>& parentMap) {
    std::unordered_set<Node*> visited;
    std::stack<Node*> stack;

    stack.push(start);
    parentMap[start] = nullptr;

    while (!stack.empty()) {
        Node* current = stack.top();
        stack.pop();

        if (visited.count(current)) continue;

        visited.insert(current);
        stepsOut.push_back(current);

        const auto& neighbors = current->getNextNodes();
        for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
            Node* neighbor = *it;
            if (!visited.count(neighbor)) {
                stack.push(neighbor);
                if (!parentMap.count(neighbor)) {
                    parentMap[neighbor] = current;
                }
            }
        }
    }
}



// DRAWING FUCNTIONS 
void draw_node(Node& node)
{
    if (node.getId() == "") {
        return;
    }
    const float radius = 20.0f;
    Vector2 pos = node.getPosition();
    for (Node* neighbor : node.getNextNodes()) {
        Vector2 direction = Vector2Subtract(neighbor->getPosition(), pos);
        Vector2 unitDir = Vector2Normalize(direction);
        Vector2 scaledDown = Vector2Scale(unitDir, radius);
        Vector2 start = Vector2Add(pos, scaledDown);
        Vector2 end = Vector2Subtract(neighbor->getPosition(), scaledDown);
        DrawLineV(start, end, BLACK);
    }
    



    Color color = GRAY;
    std::string label = node.getId();

    switch (node.getState())
    {
        case State::UNVISITED: color = GRAY; break;
        case State::VISITING: color = YELLOW; break;
        case State::VISITED: color = ORANGE; break;
        case State::PATH: color = GREEN; break;
        break;
    
    default:
        break;
    }
    
    if ( label == "-1")
    {
        label = "End";
        color = RED;
    }

    DrawCircleV(pos, radius, color);
    DrawText(label.c_str(), pos.x - 7, pos.y - 7, 20, BLACK);
}

