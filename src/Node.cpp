#include "../includes/Node.hpp"

Node::Node() : _size(0), _zeroPos(0), _cost(0), _heuristic(0), _parent(nullptr), _action("") {}

Node::Node(const std::vector<uint8_t>& state,
           int size,
           uint8_t zeroPos,
           int cost,
           int heuristic,
           std::shared_ptr<Node> parent,
           const std::string& action)
    : _state(state), _size(size), _zeroPos(zeroPos), 
      _cost(cost), _heuristic(heuristic), _parent(parent), _action(action) {}

Node::~Node() {}

Node::Node(const Node& other)
    : _state(other._state), _size(other._size), _zeroPos(other._zeroPos),
      _cost(other._cost), _heuristic(other._heuristic),
      _parent(other._parent), _action(other._action) {}

Node& Node::operator=(const Node& other) {
    if (this != &other) {
        _state = other._state;
        _size = other._size;
        _zeroPos = other._zeroPos;
        _cost = other._cost;
        _heuristic = other._heuristic;
        _parent = other._parent;
        _action = other._action;
    }
    return *this;
}

bool Node::operator<(const Node& other) const {
    // For max heap (priority queue uses max heap by default)
    // We want min heap, so reverse the comparison
    return getFValue() > other.getFValue();
}

bool Node::operator>(const Node& other) const {
    return getFValue() < other.getFValue();
}

const std::vector<uint8_t>& Node::getState() const {
    return _state;
}

int Node::getSize() const {
    return _size;
}

uint8_t Node::getZeroPos() const {
    return _zeroPos;
}

int Node::getCost() const {
    return _cost;
}

int Node::getHeuristic() const {
    return _heuristic;
}

int Node::getFValue() const {
    return _cost + _heuristic;
}

std::shared_ptr<Node> Node::getParent() const {
    return _parent;
}

const std::string& Node::getAction() const {
    return _action;
}

size_t Node::hash() const {
    size_t hash = 0;
    
    // Optimized hash for flat array using FNV-1a algorithm
    const size_t FNV_PRIME = 0x01000193;
    const size_t FNV_OFFSET = 0x811c9dc5;
    
    hash = FNV_OFFSET;
    for (size_t i = 0; i < _state.size(); i++) {
        hash ^= _state[i];
        hash *= FNV_PRIME;
    }
    
    return hash;
}

