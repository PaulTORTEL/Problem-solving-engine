#include "Sum.h"

Sum::Sum(std::vector<VarCoeff> involvedVars, std::string op, std::string type, int resultNumber, Variable* resultVar)
{
    for (VarCoeff v : involvedVars)
        _involvedVars.push_back(v);

    _operator = op;
    _type = type;

    if (_type == "number")
        _resultNumber = resultNumber;
    else if (_type == "var")
        _resultVar = resultVar;
}

Sum::~Sum()
{
    //dtor
}

bool Sum::checkInvolvedVar(const VarID& variableID) const {

    for (VarCoeff varCoeff : _involvedVars) {
        if (varCoeff.var == variableID)
            return true;
    }
    return false;
}

std::vector<VarID> Sum::getInvolvedVars() const {
    std::vector<VarID> involvedVarIDs;

    for (VarCoeff v : _involvedVars)
        involvedVarIDs.push_back(v.var);

    return involvedVarIDs;
}

bool Sum::isValuesPossibleForSum(std::vector<VarValue> varsValues) {

    std::vector<int> values;

    for (VarValue v : varsValues) {
        for (VarCoeff vc : _involvedVars) {
            if (v.var == vc.var) {
                int partial_result = v.value * vc.coeff;
                values.push_back(partial_result);
            }
        }
    }

    int result = 0;
    for (int value : values) // x - y - z ?
            result += value;

    if (_type == "number") { // X1 + X2 = NUMBER
        if (_operator == "=") {
            if (result == _resultNumber)
                return true;
        }
        else if (_operator == "<") {
            if (result < _resultNumber)
                return true;
        }
        else if (_operator == ">") {
            if (result > _resultNumber)
                return true;
        }
        else if (_operator == "<=") {
            if (result <= _resultNumber)
                return true;
        }
        else if (_operator == ">=") {
            if (result >= _resultNumber)
                return true;
        }
        else if (_operator == "!=") {
            if (result != _resultNumber)
                return true;
        }
    }


    else if (_type == "var") {  // X1 + X2 = X3
        Domain& d = _resultVar->getDomain();

        if (_operator == "=") {
            if (d.contains(result))
                return true;
        }
        else if (_operator == "<") {
            if (result < d.getMin())
                return true;
        }
        else if (_operator == ">") {
            if (result > d.getMax())
                return true;
        }
        else if (_operator == "<=") {
            if (result <= d.getMax())
                return true;
        }
        else if (_operator == ">=") {
            if (result >= d.getMin())
                return true;
        }
        else if (_operator == "!=") {
            if (!d.contains(result))
                return true;
        }
    }

    return false;
}
