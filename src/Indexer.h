#pragma once

#include <iostream>
#include <vector>
class Indexer
{

    public:
        Indexer();

        int numIndices() const;

        void addIndex(int max, int min = 1);

        bool hasNext() const;
        std::string next();

        void reset();


    private:
        std::vector<int> _maxes;
        std::vector<int> _mins;
        std::vector<int> _values;

        void advance();
};
