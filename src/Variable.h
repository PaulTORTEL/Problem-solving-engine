#ifndef VARIABLE_H
#define VARIABLE_H
#include <iostream>
#include <vector>
#include "Domain.h"
#include <string>
#include <map>

class Variable
{
    private:


    std::string _name;

    Domain _domain;


    public:

        Variable();
        Domain& getDomain();
        virtual ~Variable();


};

#endif // VARIABLE_H
