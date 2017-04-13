#ifndef INDEXREF_H
#define INDEXREF_H
#include <iostream>

class IndexRef
{

    public:
        IndexRef(std::string strSymb, std::string strValue);
        int getIndex(int origin);
        virtual ~IndexRef();

    private:

        std::string _symb;
        int _value;
};

#endif // INDEXREF_H
