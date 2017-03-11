#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <vector>
#include <map>
#include "Variable.h"
#include "Node.h"

class Engine
{
    private:

    std::string _fileName;

    std::vector<Variable> _variables;

    Node *_root;



    public:
        Engine(std::string fileName);

        bool readVariables();
        void createDumbTree(int index);

        virtual ~Engine();


};

#endif // ENGINE_H
