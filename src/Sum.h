#ifndef SUM_H
#define SUM_H

#include <vector>
#include "Variable.h"

struct VarCoeff {
    VarID var;
    int coeff;
};

struct VarValue {
    VarID var;
    int value;
};

class Sum
{

    std::vector<VarCoeff> _involvedVars;
    std::string _operator;

    std::string _type;
    int _resultNumber;
    Variable* _resultVar;

    public:
        Sum(std::vector<VarCoeff> involvedVars, std::string op, std::string type, int resultNumber, Variable* resultVar);
        ~Sum();
        bool checkInvolvedVar(const VarID& var) const;

        inline std::string getOp();
        inline std::string getType();
        inline Variable* getResultVar();
        inline int getResultNumber();
        std::vector<VarID> getInvolvedVars() const;
        bool isValuesPossibleForSum(std::vector<VarValue> varsValues);

};


inline std::string Sum::getOp() {
    return _operator;
}

inline std::string Sum::getType() {
    return _type;
}

inline Variable* Sum::getResultVar() {
    return _resultVar;
}

inline int Sum::getResultNumber() {
    return _resultNumber;
}


#endif // SUM_H
