#include "Variable.h"

Variable::Variable(Domain d, std::string& name) : _name(name), _domain(d)
{

}

Variable::~Variable() {

}

std::ostream& operator<<(std::ostream& o, const Variable& v) {
	o << "{ name : ";

    o << v._name;

    o << ", [ ";

    o << v._domain;

    o << " ]";

	return o << "}";

}

Domain& Variable::getDomain() {
    return _domain;
}
