#include "Indexer.h"
#include <sstream>

Indexer::Indexer()
{
}


int Indexer::numIndices() const {
    return _maxes.size();
}


void Indexer::addIndex(int max, int min)
{
    _mins.push_back(min);
    _maxes.push_back(max);
    _values.push_back(_values.empty() ? (min - 1) : min);
}

void Indexer::reset() {

    for (unsigned int i = 0; i < _values.size(); i++) {
        _values[i] = (i == 0 ? (_mins[i] - 1) : _mins[i]);
    }
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
                _values[k] = _mins[k];
                k++;
                while (true) {
                    if (_values[k] < _maxes[k]) {
                        _values[k]++;
                        return;
                    } else {
                        _values[k] = _mins[k];
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

bool Indexer::hasNext() const
{
    for (unsigned int i = 0; i < _maxes.size(); i++) {
        if (_values[i] < _maxes[i])
            return true;
    }
    return false;
}
