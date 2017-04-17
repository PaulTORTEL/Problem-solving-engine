#include "Constraints.h"
#include <map>
#include <stdlib.h>

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

    const std::vector<Sum> sums = getSumsWhereVarIsInvolved(var);

    for (Sum s : sums) {

        std::vector<VarID> involvedVars = s.getInvolvedVars();
        std::vector<VarValue> varsValues;

        VarValue v = {var, value};
        varsValues.push_back(v);

        for (std::vector<VarID>::iterator it = involvedVars.begin(); it != involvedVars.end(); ++it) {
            if (*it == var) {
                involvedVars.erase(it);
                break;
            }
        }

        if (involvedVars.size() > 0) {
            float count = 1.0;
            for (VarID v : involvedVars) {
                if (domains[v].isSingleton())
                    count++;

            }

            if ((count / involvedVars.size()) > 0.7) {
                if(!testCombinationForSum(domains, s, varsValues, involvedVars, 0)) {
                    return false;
                }
            }
        }
        else {
            if(!s.isValuesPossibleForSum(varsValues))
                return false;
        }
    }

	return true;
}

int Constraints::getIndexOf(int n, int m) const {
	return n*_varsNum + m;
}

const std::vector<int> Constraints::getVariablesIndexOrderedByMostOrLeastConstrained(const std::vector<Variable>& vars, bool most) {

    std::vector<VarID> result; // Stocke les index des variables selon leur nombre de contraintes respectives dans l'ordre décroissant
    std::map<VarID, int> temp_map; // (ex Var8 à 6 contraintes, Var2 à 1 contraintes => [IndexVar8][IndexVar2]...

    int count = 0;

    for (int i = 0; i < _varsNum; i++) {
        for (int j = 0; j < _varsNum; j++) {
            if (_binConstraints[getIndexOf(i,j)] != BIN_CON_ALL)
                count++;
        }
        const std::vector<Sum> sums = getSumsWhereVarIsInvolved(i);
        count += sums.size();
        temp_map[i] = count; // On compte le nombre de contrainte pour chaque variable
        count = 0;
    }

    // Ici on va insérer à la bonne place l'index afin qu'on récupère les index des vars ayant le + de contraintes au - de contraintes
    for (auto it = temp_map.begin(); it != temp_map.end(); ++it) {

        if (result.empty()) // Si c'est le premier élément, rien à comparer donc on push
            result.push_back(it->first);

        else {
            for (auto it2 = result.begin(); it2 != result.end(); ++it2) { // On parcourt ce qu'on a déjà trié

                if (most) { // On veut les + contraintes d'abord
                    if (it->second >= temp_map.find(*it2)->second) { // On compare le nb de contraintes de ceux triés avec celui qu'on veut insérer
                        result.insert(it2, it->first);
                        break;
                    }

                    else if (it2 + 1 == result.end()) { // Traite le + petit élément
                        result.insert(it2 + 1, it->first);
                        break;
                    }
                }
                else { // On veut les moins contraintes d'abord
                    if (it->second <= temp_map.find(*it2)->second) { // On compare le nb de contraintes de ceux triés avec celui qu'on veut insérer
                        result.insert(it2, it->first);
                        break;
                    }

                    else if (it2 + 1 == result.end()) { // Traite le + petit élément
                        result.insert(it2 + 1, it->first);
                        break;
                    }
                }
            }
        }
    }

    return result;
}

void Constraints::addSumConstraint(std::vector<VarCoeff> involvedVars, std::string op, std::string type, int resultNumber, Variable* resultVar, VarID resultVarID) {
    _sumConstraints.push_back(Sum(involvedVars, op, type, resultNumber, resultVar, resultVarID));
}

std::vector<Sum> Constraints::getSumsWhereVarIsInvolved(const VarID& v) const {

    std::vector<Sum> allSum;
    for (unsigned int i = 0; i < _sumConstraints.size(); i++) {
        if (_sumConstraints[i].checkInvolvedVar(v))
            allSum.push_back(_sumConstraints[i]);
    }
    return allSum;
}

bool Constraints::testCombinationForSum(std::vector<Domain>& domains, Sum& s, std::vector<VarValue>& values_test, const std::vector<VarID>& varsID, unsigned int index) const {

    if (index < varsID.size()) {

        Domain& d = domains[varsID[index]];

        for (unsigned int cpt = 0; cpt < d.getSize(); cpt++) {

            VarValue v = {varsID[index], d[cpt]};
            values_test.push_back(v);

            if (!testCombinationForSum(domains, s, values_test, varsID, index+1))
                values_test.pop_back();

            else
                return true;
        }
    }
    else {
        if(s.isValuesPossibleForSum(values_test))
            return true;
    }

    return false;
}
