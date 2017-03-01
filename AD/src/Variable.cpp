#include "Variable.h"

Variable::Variable()
{

}

Variable::~Variable()
{

}

bool Variable::CheckValue(int n, int dim) {
        dim-=1;
        if (dim > 0 && dim < _domain.size()) {
            return _domain[dim]->inDomain(n);
        }
       return false;
}

unsigned int Variable::getDomainDimension() {
    return _domain.size();
}

void Variable::setDomain(std::map<int, VariableDomain*> domain) {
    _domain = domain;
}
