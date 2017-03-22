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

        // 1er vect des variables => réduc domaine, 2e => pour récupérer
        bool createNode(int index, std::vector<Variable> vars, std::vector<int>& chosenValues, Constraints* constraints);

        void addChild(Node * new_node);
        bool reduceDomains(std::vector<int> const & chosenValues, std::vector<Variable>& vars, Constraints* constraints);
        bool edgeConsistency(std::vector<Variable>& vars, Constraints* constraints, unsigned int index, std::vector<int>& already_treated);


        inline unsigned int getCount();
        inline unsigned int getCountPruned();

};

inline unsigned int Node::getCount() {
    return _count;
}
