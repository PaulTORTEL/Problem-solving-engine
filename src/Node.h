#ifndef NODE_H
#define NODE_H
#include "Variable.h"
#include <iostream>
#include <vector>

class Node
{
    private:

    std::vector<Node*> _childs;

    int _index; // index dans le vector des variables dans Engine.h
    int _value;

    public:
        Node(int index  = -1);
        virtual ~Node();
        bool createDumbNode(int index, std::vector<Variable>& vars, std::vector<int>& chosenValues);
        void addChild(Node * new_node);

};

#endif // NODE_H
