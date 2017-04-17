#include "Variable.h"

Variable::Variable(Domain d, const std::string& name) : _name(name), _domain(d), _level(-1)
{

}

std::ostream& operator<<(std::ostream& o, const Variable& v) {
	o << v._name;
	o << " : " << v._domain;
	return o;

}

const std::string& Variable::getName() const {
	return _name;
}

Domain& Variable::getDomain() {
    return _domain;
}

int Variable::getLevel() const {
    return _level;
}

void Variable::setLevel(int level) {
    this->_level = level;
}
