#include "Constraints.h"
#include <algorithm>
#include <map>

Constraints::Constraints(int n):
	_varsNum(n),
	//Sans le '+' ça ne compile pas sous gcc/clang
	_binConstraints(n*n, +BIN_CON_ALL) {
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
	std::cout << BIN_CON_ALL;
	if(!(constraint & BIN_CON_LESS) && v1 < v2)
		return false;

	if(!(constraint & BIN_CON_EQUALS) && v1 == v2)
		return false;

	if(!(constraint & BIN_CON_GREATER) && v1 > v2)
		return false;

	return true;
}


bool Constraints::isValuePossible(std::vector<Domain>& domains, VarID var, int value) const {
	Domain& d1 = domains[var];
	if(!d1.contains(value))
		return false;

	for(int i = 0; i < _varsNum; i++) {
		char c = _binConstraints[getIndexOf(var, i)];
		Domain& d2 = domains[i];

		if(!(c & BIN_CON_LESS) && value < d2.getMin())
			return false;

		if(!(c & BIN_CON_GREATER) && value > d2.getMax())
			return false;

		if(!(c & BIN_CON_EQUALS) && d2.getMax() == value && d2.getMin() == value)
			return false;
	}

	return true;
}

int Constraints::getIndexOf(int n, int m) const {
	return n*_varsNum + m;
}

std::vector<int> Constraints::getRelatedVariablesIndex(unsigned int n) {

    std::vector<int> temp;
    if ((int) n >= _varsNum)
        return temp;

    for (int i = 0; i < _varsNum; i++) {
        if (_binConstraints[getIndexOf(n, i)] != BIN_CON_ALL)
            temp.push_back(i);
    }

    return temp;
}

const std::vector<int> Constraints::getVariablesIndexOrderedByMostConstrained() {

    // 1 : compter toutes les contraintes déclarées par var
    // 2 : trier le tab
    // 3 : le renvoyer
 //   std::map<int, int> temp_map;
    std::vector<int> temp;
    unsigned int count = 0;

    for (int i = 0; i < _varsNum; i++) {
        for (int j = 0; j < _varsNum; j++) {
            if (_binConstraints[getIndexOf(i,j)] != BIN_CON_ALL)
                count++;
        }
        //temp_map[i] = count;
        temp.push_back(count);
        count = 0;
    }

    std::sort(temp.begin(), temp.end()); // Trie par ordre croissant de base
    std::reverse(temp.begin(), temp.end()); // On veut du + contraint au - donc on reverse le tri croissant

 // PB : ON NE SAIT PAS QUI EST placé où !! comme on a tout trié !!
    return temp;
}

