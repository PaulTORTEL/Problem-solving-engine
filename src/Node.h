#ifndef NODE_H
#define NODE_H
#include "Variable.h"
#include <iostream>
#include <vector>


class Node
{
    private:

    std::vector<Node*> _childs;

    Variable *_variable;

    std::vector<int> _value;

    public:
        Node();
        virtual ~Node();

};

#endif // NODE_H
