#pragma once

#include <iostream>
#include <vector>
#include "Domain.h"
#include <string>
#include <map>

typedef int VarID;

class Variable
{
    private:


    std::string _name;
    Domain _domain;


    public:

        const std::string& getName() const;
        Domain& getDomain();
        friend std::ostream& operator<<(std::ostream& o, const Variable& v);


        Variable(Domain d, std::string& name);

};

