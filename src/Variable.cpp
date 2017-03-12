#include "Variable.h"

Variable::Variable(Domain d, std::string& name) : _name(name), _domain(d)
{

}

std::ostream& operator<<(std::ostream& o, const Variable& v) {
	o << "{ name : " << v._name;
	o << ", [ " << v._domain << " ]";
	return o << "}";

}

const std::string& Variable::getName() const {
	return _name;
}

Domain& Variable::getDomain() {
    return _domain;
}
