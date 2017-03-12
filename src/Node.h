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
    int _value;

    public:
        Node(int index  = -1);
        virtual ~Node();
        bool createDumbNode(int index, std::vector<Variable> vars, std::vector<int>& chosenValues, Constraints* constraints);
        void addChild(Node * new_node);

};
