#ifndef NODE_HPP
#define NODE_HPP

#include <vector>
#include <string>
#include <memory>
#include <cstdint>

class Node {
    public:
        Node();
        Node(const std::vector<uint8_t>& state,
             int size,
             uint8_t zeroPos,
             int cost,
             int heuristic);
        ~Node();
        Node(const Node& other);
        Node& operator=(const Node& other);

        // Comparison operators for priority queue
        bool operator<(const Node& other) const;
        bool operator>(const Node& other) const;

        // Getters
        const std::vector<uint8_t>& getState() const;
        int getSize() const;
        uint8_t getZeroPos() const;
        int getCost() const;
        int getHeuristic() const;
        int getFValue() const;

        // Hash function for closed set
        size_t hash() const;

    private:
        std::vector<uint8_t> _state;  // Flat array representation
        int _size;                     // Grid dimension (e.g., 3 for 3x3)
        uint8_t _zeroPos;              // Position of blank tile (0-based flat index)
        int _cost;       // g(n) - cost from start
        int _heuristic;  // h(n) - heuristic estimate
        
        // MEMORY OPTIMIZATION: Removed _parent and _action
        // We only need the cost to reach goal, not the full path
        // This reduces memory per node from ~130 bytes to ~60 bytes (2x improvement!)
};

#endif
