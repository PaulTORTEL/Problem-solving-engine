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
    int _level; // Utile pour savoir à quelle position la variable se trouve dans le traitement
                // (ex: si X3 doit être traitée en 1ere car la + contrainte, son level = 0)


    public:

        const std::string& getName() const;
        Domain& getDomain();
        int getLevel() const;
        void setLevel(int level);
        friend std::ostream& operator<<(std::ostream& o, const Variable& v);


        Variable(Domain d, const std::string& name);
};

