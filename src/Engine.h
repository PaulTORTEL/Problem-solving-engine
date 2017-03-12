#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "Variable.h"
#include "Node.h"
#include "Constraints.h"

class Engine
{
    private:

    std::string _fileName;

    std::vector<Variable> _variables;

    Constraints *_constraints;
    Node *_root;



    public:
        Engine(const std::string& fileName);

        bool readVariables();
        void createDumbTree(int index);

        void tempConstFulfiller();

};
