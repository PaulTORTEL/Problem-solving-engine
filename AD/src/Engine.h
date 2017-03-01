#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "Variable.h"

class Engine
{
    private:

    std::string _fileName;

    std::map<std::string,Variable*> _variables;

    public:
        Engine(std::string fileName);

        bool readVariables();

        virtual ~Engine();


};

#endif // ENGINE_H
