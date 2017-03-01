#ifndef VARIABLEDOMAIN_H
#define VARIABLEDOMAIN_H

#include "Domain.h"
#include <iostream>
#include <vector>

class VariableDomain
{
    private:
    int _order;
    std::vector<Domain*> _domains;

    public:

        VariableDomain(int order);

        void AddSubDomain(Domain* d);
        bool inDomain(int n);

        virtual ~VariableDomain();

};

#endif // VARIABLEDOMAIN_H
