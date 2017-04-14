#include "Sum.h"
#include <stdlib.h>

Sum::Sum(std::vector<VarCoeff> involvedVars, std::string op, std::string type, int resultNumber, Variable* resultVar, VarID resultVarID)
{
    for (VarCoeff v : involvedVars)
        _involvedVars.push_back(v);

    _operator = op;
    _type = type;

    if (_type == "number")
        _resultNumber = resultNumber;
    else if (_type == "var")
        _resultVar = resultVar;

    _resultVarID = resultVarID;

//std::cout << involvedVars.size() << " -> " << op << " / " << type << " / " << resultNumber << " / " << resultVar << " / " << resultVarID << std::endl;

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

    if (_type == "var") {
        if (_resultVarID == variableID)
            return true;
    }

    return false;
}

std::vector<VarID> Sum::getInvolvedVars() const {
    std::vector<VarID> involvedVarIDs;

    for (VarCoeff v : _involvedVars)
        involvedVarIDs.push_back(v.var);

    if (_type == "var")
        involvedVarIDs.push_back(_resultVarID);

    return involvedVarIDs;
}

bool Sum::isValuesPossibleForSum(std::vector<VarValue> varsValues) {

    int result = 0;

    for (VarValue v : varsValues) {
        for (VarCoeff vc : _involvedVars) {
            if (_type == "var") {
                if ((v.var != _resultVarID) && (v.var == vc.var)) {
                    int partial_result = v.value * vc.coeff;
                    result += partial_result;
                }
            }
            else if (_type == "number") {
                if (v.var == vc.var) {
                   // std::cout << v.var << " => " << v.value << std::endl;
                    int partial_result = v.value * vc.coeff;
                    result += partial_result;
                }
            }
        }
    }

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
            if (result > _resultNumber) {
                //std::cout << "true! " << std::endl;
                return true;
            }
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

        int temp_test = -1;

        for (VarValue v : varsValues) {
            if (v.var == _resultVarID)
                temp_test = v.value;
        }

        if (_operator == "=") {
            //if (d.contains(result))
            if (result == temp_test)
                return true;
        }
        else if (_operator == "<") {
            //if (result < d.getMin())
            if (result < temp_test)
                return true;
        }
        else if (_operator == ">") {
            //if (result > d.getMax())
            if (result > temp_test)
                return true;
        }
        else if (_operator == "<=") {
            //if (result <= d.getMax())
            if (result <= temp_test)
                return true;
        }
        else if (_operator == ">=") {
            //if (result >= d.getMin())
            if (result >= temp_test)
                return true;
        }
        else if (_operator == "!=") {
            //if (!d.contains(result))
            if (result != temp_test)
                return true;
        }
    }

    return false;
}
