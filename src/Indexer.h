#pragma once

#include <iostream>
#include <vector>
class Indexer
{

    public:
        Indexer();

        void addIndex(int max);

        bool hasNext();
        void next();
        std::string getVarIndex();


    private:
        std::vector<int> _maxes;
        std::vector<int> _values;
};
