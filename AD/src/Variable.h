#ifndef VARIABLE_H
#define VARIABLE_H
#include <iostream>
#include <vector>
#include "VariableDomain.h"
#include <string>
#include <map>

class Variable
{
    private:


    std::string _name;

    std::map<int, VariableDomain*> _domain;


    public:

        Variable();

        bool CheckValue(int n, int dim = 0);

        unsigned int getDomainDimension();

        void setDomain(std::map<int, VariableDomain*> domain);

        virtual ~Variable();


};

#endif // VARIABLE_H
