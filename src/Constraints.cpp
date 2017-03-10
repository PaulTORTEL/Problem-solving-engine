#include "Constraints.h"

#include <cstring>

Constraints::Constraints(int n): _varsNum(n) {
	_binConstraints = new char[n*n];
	memset(_binConstraints, BIN_CON_ALL, sizeof(char[n*n]));
}

Constraints::~Constraints() {
	delete[] _binConstraints;
}

void Constraints::addBinConstraint(int v1, int v2, char constraint) {
	_binConstraints[getIndexOf(v1, v2)] &= constraint;
	
	char inv = BIN_CON_NONE;
	if(constraint & BIN_CON_LESS)
		inv |= BIN_CON_GREATER;
	if(constraint & BIN_CON_EQUALS)
		inv |= BIN_CON_EQUALS;
	if(constraint & BIN_CON_GREATER)
		inv |= BIN_CON_LESS;

	_binConstraints[getIndexOf(v2, v1)] &= inv;
}

bool Constraints::checkBinary(int v1, int v2, char constraint) {
	if(!(constraint & BIN_CON_LESS) && v1 < v2)
		return false;

	if(!(constraint & BIN_CON_EQUALS) && v1 == v2)
		return false;

	if(!(constraint & BIN_CON_GREATER) && v1 > v2)
		return false;

	return true;
}


bool Constraints::isValuePossible(std::vector<Domain>& vars, VarID var, int value) const {
	Domain& d1 = vars[var];
	if(!d1.contains(value))
		return false;

	for(int i = 0; i < _varsNum; i++) {
		char c = _binConstraints[getIndexOf(var, i)];
		Domain& d2 = vars[i];

		if(!(c & BIN_CON_LESS) && value >= d2.getMax())
			return false;
		
		if(!(c & BIN_CON_GREATER) && value <= d2.getMin())
			return false;

		if(!(c & BIN_CON_EQUALS) && !d2.contains(value))
			return false;
	}

	return true;
}

int Constraints::getIndexOf(int n, int m) const {
	return n*_varsNum + m;
}