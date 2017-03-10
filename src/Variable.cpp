#include "Variable.h"

Variable::Variable(Domain d, std::string& name) : _domain(d), _name(name)
{

}

Variable::~Variable()
{

}

std::ostream& operator<<(std::ostream& o, const Variable& v) {
	o << "{ name : ";

    o << v._name;

    o << ", [ ";

    o << v._domain;

    o << " ]";

	return o << "}";
}
