#include "Constraints.h"
#include <map>
#include <stdlib.h>

Constraints::Constraints(int n):
	_varsNum(n),
	//Sans le '+' ça ne compile pas sous gcc/clang
	_binConstraints(n*n, +BIN_CON_ALL) {
}

void Constraints::addBinConstraint(int v1, int v2, char constraint) {

    //  std::cout << v1 << " - " << v2 << " / " << ((int)constraint) << std::endl;

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
//std::cout << "\t\t\t\t TEST SUR " << var << " ==> " << value << std::endl;
    //for (Domain d : domains)
       // std::cout << d << std::endl;

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
        //std::cout << "on analyse " << var << " on lui donne val " << v.value << " et le dom : " << domains[involvedVars[1]] << std::endl;
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
                    //        for (Domain d : domains)
          //  std::cout << d << std::endl;
        //system("pause");
                   // std::cout << "ok on test " << (involvedVars.size() / count) << std::endl;
                if(!testCombinationForSum(domains, s, varsValues, involvedVars, 0)) {
                    //std::cout << "retourne faux : var " << var << " => " << value << " impossible et sera suppr du domaine" << std::endl;
                    return false;
                }
            }
        }
        else {
            if(!s.isValuesPossibleForSum(varsValues))
                return false;
        }
       // for (VarValue vv : varsValues)
          //  std::cout << vv.var << " prend " << vv.value << std::endl;
    }

 //std::cout << std::endl << "on passe avec " << var << " pour val : " << value << std::endl;
// system("pause");
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
    /*std::cout << "LE TAB : " << std::endl;
    for (VarID vv : result)
        std::cout << vv << std::endl;*/
    return result;
}

void Constraints::addSumConstraint(std::vector<VarCoeff> involvedVars, std::string op, std::string type, int resultNumber, Variable* resultVar, VarID resultVarID) {
    _sumConstraints.push_back(Sum(involvedVars, op, type, resultNumber, resultVar, resultVarID));
}

/// PEUT ETRE INUTILE !!!
const std::vector<VarID> Constraints::getRelatedVars(VarID index) {

    std::vector<VarID> relatedVars;

    for(int i = 0; i < _varsNum; i++) {
		char c = _binConstraints[getIndexOf(i, index)];

		if (c == (BIN_CON_GREATER | BIN_CON_LESS))
            relatedVars.push_back(i);
	}

	std::vector<Sum> sums = getSumsWhereVarIsInvolved(index);

	for (std::vector<Sum>::iterator it = sums.begin(); it != sums.end(); ++it) {

        std::vector<VarID> varsInvolved = it->getInvolvedVars();

        for (std::vector<VarID>::iterator it_var = varsInvolved.begin(); it_var != varsInvolved.end(); ++it_var) {
            if (*it_var != index) {
                bool success = false;
                for (std::vector<VarID>::iterator it_relat_var = relatedVars.begin(); it_relat_var != relatedVars.end(); ++it_relat_var) {
                    if (*it_relat_var == *it_var) {
                        success = true;
                        break;
                    }
                }

                if (!success)
                    relatedVars.push_back(*it_var);
            }
        }
	}
	return relatedVars;
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

    //std::cout << "index : " << index << " et vars : " << varsID.size() << std::endl;
   // system("pause");
    if (index < varsID.size()) {
       // std::cout << "\t\tle varID [index] : " << varsID[index] << std::endl;

        Domain& d = domains[varsID[index]];
       // std::cout << "\t la taille du dom : " << d.getSize() << std::endl;
       for (unsigned int cpt = 0; cpt < d.getSize(); cpt++) {
          //  std::cout << "la struc : " << varsID[index] << " value  " << d[cpt] << std::endl;
            VarValue v = {varsID[index], d[cpt]};

            values_test.push_back(v);

            if (!testCombinationForSum(domains, s, values_test, varsID, index+1))
                values_test.pop_back();

            else
                return true;
        }

        //std::cout << " c'est " << varsID[index] << " qui renvoie faux " << index << "et dom size : " << d.getSize() << std::endl;
    }
    else {
           // std::cout << " on va tester la combi " << std::endl;
            //std::cout << "l'index : " << index << " et la struc : " << values_test[1].value << std::endl;
        if(s.isValuesPossibleForSum(values_test))
            return true;
    }
    //std::cout << "on return false" << std::endl;
    return false;
}

/* PARTIE NON FONCTIONNELLE, IDEE POUR OPTIMISER MAIS SANS GRAND SUCCES .... */
/*
bool Constraints::testCombinationForSum(std::vector<Domain>& domains, Sum& s, std::vector<VarValue>& values_test, const std::vector<VarID>& varsID, unsigned int index) const {

    if (s.getOp() == ">") {
        for (unsigned int i = 0; i < varsID.size(); i++) {
            VarValue v = {varsID[i], domains[varsID[i]].getMax()};
            values_test.push_back(v);
        }

        if(s.isValuesPossibleForSum(values_test)) {
            return true;
        }
    }
    else if (s.getOp() == "<") {
        for (unsigned int i = 0; i < varsID.size(); i++) {
            VarValue v = {varsID[i], domains[varsID[i]].getMin()};
            values_test.push_back(v);
        }
        if(s.isValuesPossibleForSum(values_test))
            return true;
    }
    else if (s.getOp() == "=") {
         if (index < varsID.size()) {
            //std::cout << "\t\tle varID [index] : " << varsID[index] << std::endl;

            Domain& d = domains[varsID[index]];
            //std::cout << "\t la taille du dom : " << d.getSize() << " (pour " << varsID[index] << std::endl;
            for (Domain::iterator it = d.begin(); it != d.end(); ++it) {
                //std::cout << "la struc : " << varsID[index] << " value  " << it.getValue() << std::endl;
                VarValue v = {varsID[index], it.getValue()};

                values_test.push_back(v);
               // std::cout << " et la value importante : " << it.getValue() << " et la struc : " << values_test.size() << std::endl;
                if (!testCombinationForSum(domains, s, values_test, varsID, index+1))
                    values_test.pop_back();

                else
                    return true;
            }

            //std::cout << " c'est " << varsID[index] << " qui renvoie faux " << index << "et dom size : " << d.getSize() << std::endl;
        }
        else {
                //std::cout << "l'index : " << index << " et la struc : " << values_test[1].value << std::endl;
            if(s.isValuesPossibleForSum(values_test))
                return true;
        }
    }
    return false;
}*/
