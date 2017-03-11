#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "Variable.h"

class Engine
{
    private:

    std::string _fileName;

    std::vector<Variable> _variables;

    public:
        Engine(const std::string& fileName);

        bool readVariables();


};
