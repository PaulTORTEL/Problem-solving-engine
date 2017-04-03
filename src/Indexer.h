#pragma once

#include <iostream>
#include <vector>
class Indexer
{

    public:
        Indexer();

        int numIndices() const;

        void addIndex(int max);

        bool hasNext() const;
        std::string next();


    private:
        std::vector<int> _maxes;
        std::vector<int> _values;

        void advance();
};
