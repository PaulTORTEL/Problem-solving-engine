#pragma once

#include "Variable.h"
#include <iostream>
#include <vector>
#include "Constraints.h"

class Node
{
    private:

    std::vector<Node*> _children;
    int _index; // index dans le vector des variables dans Engine.h
    static unsigned int _count;

    static float _countPruning;
    static float _countDepth;
    static unsigned int _maxPruningDepth;

    public:

        Node(int index  = -1);
        virtual ~Node();

        // 1er vect des variables => réduc domaine, 2e => pour récupérer
        bool createNode(int index, std::vector<Variable> vars, std::vector<int>& chosenValues, Constraints* constraints, const bool& domain_method);

        void addChild(Node * new_node);
        bool reduceDomains(std::vector<Variable>& vars, Constraints* constraints);
        bool edgeConsistency(std::vector<Variable> vars, Constraints* constraints, unsigned int index, int valuePossibleNextLevel);
        void sortChosenValuesByNaturalOrder(const std::vector<Variable>& vars, std::vector<int>& chosenValues);

        inline unsigned int getCount();
        inline float getCountPruning();
        inline float getCountDepth();
        inline unsigned int getMaxPruningDepth();

        inline void refreshCount();
        inline void refreshCountPruning();
        inline void refreshCountDepth();
        inline void refreshMaxPruningDepth();

};

inline unsigned int Node::getCount() {
    return _count;
}

inline float Node::getCountPruning() {
    return _countPruning;
}

inline float Node::getCountDepth() {
    return _countDepth;
}

inline unsigned int Node::getMaxPruningDepth() {
    return _maxPruningDepth;
}

inline void Node::refreshCount() {
    _count = 0;
}

inline void Node::refreshCountPruning() {
    _countPruning = 0.0;
}
inline void Node::refreshCountDepth() {
    _countDepth = 0.0;
}

inline void Node::refreshMaxPruningDepth() {
    _maxPruningDepth = 0;
}
