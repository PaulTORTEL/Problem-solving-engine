#pragma once

#include <vector>
#include "Variable.h"

class Constraints {

	const int _varsNum;
	std::vector<char> _binConstraints;

public:
	static const char BIN_CON_NONE = 0;
	static const char BIN_CON_LESS = 1;
	static const char BIN_CON_EQUALS = 2;
	static const char BIN_CON_GREATER = 4;
	static const char BIN_CON_ALL = 7;


	Constraints(int n);

	void addBinConstraint(int v1, int v2, char constraint);

	bool isValuePossible(std::vector<Domain>& vars, VarID var, int value) const;

	static bool checkBinary(int v1, int v2, char constraint);

	std::vector<int> getRelatedVariablesIndex(unsigned int n); // Renvoie un vecteur d'index des variables ayant une contrainte avec la variable d'index n
	const std::vector<int> getVariablesIndexOrderedByMostOrLeastConstrained(const std::vector<Variable>& vars, bool most);



private:
	int getIndexOf(int n, int m) const;

};
