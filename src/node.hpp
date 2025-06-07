#ifndef H_NODE
#define H_NODE

#include <string>
#include <vector>
#include "../raylib/src/raylib.h"

enum State
{
    UNVISITED,
    VISITING,
    VISITED,
    PATH
};

class Node {
private:
    std::string id;
    std::vector<Node*> nextNodes;  
    Vector2 position;
    State state = State::UNVISITED;

public:
    Node(std::string id) : id(std::move(id)) {}

    void addNext(Node* node) { nextNodes.push_back(node); }
    const std::vector<Node*>& getNextNodes() const { return nextNodes; }

    const std::string& getId() const { return id; }

    void setPosition(Vector2 pos) { position = pos; }
    Vector2 getPosition() const { return position; }

    void setState(State s) { state = s; }
    State getState() const {return state; } 

};

#endif
