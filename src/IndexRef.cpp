#include "IndexRef.h"
#include "Utils.h"

IndexRef::IndexRef(std::string strSymb, std::string strValue)
{
    _value = -1;
    parseNumber(strValue.c_str(),&_value);
    _symb = strSymb;
}

IndexRef::~IndexRef()
{
    //dtor
}


int IndexRef::getIndex(int origin) {

    if (_value == -1) {
        return -1;
    }

    if (_symb.length() == 0)
        return origin;

    if (_symb == "-") {
        return origin - _value;
    }


    return -1;
}
