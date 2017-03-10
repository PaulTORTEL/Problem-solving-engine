#ifndef INDEXER_H
#define INDEXER_H
#include <iostream>
#include <vector>
class Indexer
{

    public:
        Indexer();
        virtual ~Indexer();

        void addIndex(int max);

        bool hasNext();
        void next();
        std::string getVarIndex();


    private:
        std::vector<int> _maxes;
        std::vector<int> _values;
};

#endif // INDEXER_H
