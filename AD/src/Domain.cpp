#include "Domain.h"

Domain::Domain(int min, int max)
{
    _min = min;
    _max = max;
}

Domain::~Domain()
{
    //dtor
}

bool Domain::isIncluded(int n) {
    return n >= _min && n <= _max;
}
