#include "Indexer.h"
#include <sstream>

Indexer::Indexer()
{

}

Indexer::~Indexer()
{

}

void Indexer::addIndex(int max)
{
    _maxes.push_back(max);
    _values.push_back(1);
}

std::string Indexer::getVarIndex()
{
    std::string temp = "";
    std::stringstream ss;
    for (unsigned int i = 0; i < _maxes.size(); i++)
    {
        ss << "[";
        ss << _values[i];
        ss << "]";
    }
    ss >> temp;

    return temp;
}

void Indexer::next()
{
    for (unsigned int i = 0; i < _maxes.size(); i++)
    {
        if (_values[i] < _maxes[i])
        {
            _values[i]++;
            return;
        }
        else
        {
            if (i == _maxes.size() - 1)
            {
                return;
            }
            else
            {
                int k = i;
                _values[k] = 1;
                k++;
                while (true)
                {
                    if (_values[k] < _maxes[k])
                    {
                        _values[k]++;
                        return;
                    }
                    else
                    {
                        _values[k] = 1;
                        k++;
                    }
                }
            }
        }

    }
    return ;
}

bool Indexer::hasNext()
{

    for (unsigned int i = 0; i < _maxes.size(); i++)
    {
        if (_values[i] < _maxes[i])
        {
            return true;
        }
    }
    return false;
}
