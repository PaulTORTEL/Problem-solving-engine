#include "Indexer.h"
#include <sstream>

Indexer::Indexer()
{
}

void Indexer::addIndex(int max)
{
    _maxes.push_back(max);
    _values.push_back(_values.empty() ? 0 : 1);
}

void Indexer::advance() {
    for (unsigned int i = 0; i < _maxes.size(); i++) {
        if (_values[i] < _maxes[i]) {
            _values[i]++;
            return;

        } else {
            if (i == _maxes.size() - 1)
                return;
            else {
                int k = i;
                _values[k] = 1;
                k++;
                while (true) {
                    if (_values[k] < _maxes[k]) {
                        _values[k]++;
                        return;
                    } else {
                        _values[k] = 1;
                        k++;
                    }
                }
            }
        }
    }
}


std::string Indexer::next() {
    advance();

    std::stringstream ss;
    for (unsigned int i = 0; i < _maxes.size(); i++)
    {
        ss << "[";
        ss << _values[i];
        ss << "]";
    }
    return ss.str();
}

bool Indexer::hasNext()
{
    for (unsigned int i = 0; i < _maxes.size(); i++) {
        if (_values[i] < _maxes[i])
            return true;
    }
    return false;
}
