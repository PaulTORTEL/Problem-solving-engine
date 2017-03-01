#include "VariableDomain.h"

VariableDomain::VariableDomain(int order)
{
    _order = order;
}

VariableDomain::~VariableDomain()
{
    for (unsigned int i = 0; i < _domains.size(); i++) {
        delete _domains[i];
    }
}


void VariableDomain::AddSubDomain(Domain* d) {
    _domains.push_back(d);
}

bool VariableDomain::inDomain(int n) {
    for (unsigned int i = 0; i < _domains.size(); i++) {
        Domain* d = _domains[i];
        if (d->isIncluded(n))
            return true;
    }
    return false;
}
