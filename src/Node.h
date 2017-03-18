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

    public:

        Node(int index  = -1);
        virtual ~Node();
        bool createNode(int index, std::vector<Variable> vars, std::vector<int>& chosenValues, Constraints* constraints);
        void addChild(Node * new_node);
        bool reduceDomains(std::vector<int> const & chosenValues, std::vector<Variable>& vars, Constraints* constraints);

        inline unsigned int getCount();
        inline void setCount(unsigned int count);

};

inline unsigned int Node::getCount() {
    return _count;
}
