#pragma once

#include <vector>
#include "Variable.h"
#include "Sum.h"

class Constraints {

	const int _varsNum;
	std::vector<char> _binConstraints;
	std::vector<Sum> _sumConstraints;

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

	const std::vector<int> getVariablesIndexOrderedByMostOrLeastConstrained(const std::vector<Variable>& vars, bool most);


	void addSumConstraint(std::vector<VarCoeff> involvedVars, std::string op, std::string type, int resultNumber = 0, Variable* resultVar = nullptr);
	std::vector<Sum> getSumsWhereVarIsInvolved(const VarID& v) const;

	bool testCombinationForSum(std::vector<Domain>& domains, Sum& s, std::vector<VarValue>& values_test, const std::vector<VarID>& varsID, unsigned int index) const;

private:
	int getIndexOf(int n, int m) const;

};
