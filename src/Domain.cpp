
#include "Domain.h"
#include <climits>

bool Range::contains(int n) const {
	return n >= min && n <= max;
}

bool Range::isEmpty() const {
	return min > max;
}

bool Range::isSingleton() const {
	return min == max;
}

unsigned int Range::size() const {
	return isEmpty() ? 0 : max - min;
}

Range Range::from(int n) {
	return Range {n, n};
}

Range Range::fromExclusive(int min, int max) {
	return Range {min, max-1};
}

Range Range::fromInclusive(int min, int max) {
	return Range {min, max};
}

Range Range::lessThan(int max) {
	return Range {INT_MIN, max-1};
}

Range Range::lessThanOrEqualsTo(int max) {
	return Range {INT_MIN, max};
}

Range greaterThan(int max) {
	return Range {max+1, INT_MAX};
}
Range greaterThanOrEqualsTo(int max) {
	return Range {max, INT_MAX};
}

std::ostream& operator<<(std::ostream& o, const Range& r) {
	if(r.isEmpty())
		return o << ".";
	if(r.isSingleton())
		return o << r.min;
	return o << r.min << "..." << r.max;
}

int Domain::getMin() const {
	return _ranges.empty() ? INT_MAX : _ranges.front().min;
}

int Domain::getMax() const {
	return _ranges.empty() ? INT_MIN : _ranges.back().max;
}

Domain::iterator Domain::begin() const {
	return Domain::iterator(_ranges.begin(), _ranges.empty() ? INT_MIN : _ranges.front().min); 	
}

Domain::iterator Domain::end() const {
	return Domain::iterator(_ranges.end(), _ranges.empty() ? INT_MIN : _ranges.back().max);
}

Range Domain::getEnclosingRange() const {
	return Range {getMin(), getMax()};
}

bool Domain::isEmpty() const {
	return _ranges.empty();
}
bool Domain::isSingleton() const {
	return _ranges.size() == 1 && _ranges[0].isSingleton();
}

bool Domain::contains(int n) const {
	return locate(n).inside;
}

Domain::Location Domain::locate(int n) const {
	return locate(n, Domain::Location {0, false});
}

Domain::Location Domain::locate(int n, Domain::Location start) const {
	for(unsigned int i = start.idx; i < _ranges.size(); i++) {
		const Range& cur = _ranges[i];
		if(n <= cur.max) {
			bool inside = n >= cur.min;
			return Domain::Location {i, inside};
		}
	}

	return Domain::Location {(unsigned int) _ranges.size(), false};
}


bool Domain::touchPrev(Domain::Location loc, int n) {
	return !loc.inside
		&& loc.idx > 0
		&& _ranges[loc.idx-1].max == n-1;
}

bool Domain::touchNext(Domain::Location loc, int n) {
	return !loc.inside
		&& loc.idx < _ranges.size()
		&& _ranges[loc.idx].min == n+1;
}

bool Domain::clear() {
	bool empty = isEmpty();
	_size = 0;
	_ranges.clear();
	return empty;
}


void Domain::recalculateSize() {
	_size = 0;
	for (Range r: _ranges)
        _size += r.size();
}

//La fonction la plus compliquée de toutes, trouver comment simplifier ??
bool Domain::add(Range r) {
	if(r.isEmpty())
		return false;

	Domain::Location start = locate(r.min);

	//En bout de vector, on l'ajoute
	if(start.idx == _ranges.size()) {
		if(touchPrev(start, r.min))
			_ranges.back().max = r.max;
		else _ranges.push_back(r);
		_size += r.size();
		return true;
	}

	Domain::Location end = locate(r.max, start);

	//On ajuste r pour pas que ça touche les intervalles
	//A la place, on chevauche
	if(touchPrev(start, r.min)) {
		r.min--;
		start.idx--;
		start.inside = true;
	}

	if(touchNext(end, r.max)) {
		r.max++;
		end.idx++;
		end.inside = true;
	}

	//Cas spéciaux
	if(start.idx == end.idx) {
		//Disjoint de tous les intervalles, on l'ajoute
		if(!start.inside && !end.inside) {
			_ranges.insert(_ranges.begin() + start.idx, r);
			_size += r.size();
			return true;
		}

		//Contenu dans un intervalle, on fait rien
		if(start.inside && end.inside)
			return false;
	}

	Range& final = _ranges[start.idx];

	//On ajuste la borne inférieure
	if(!start.inside) {
		final.min = r.min;
	}

	//On ajuste la borne supérieure
	if(end.inside) {
		final.max = _ranges[end.idx].max;
		end.idx++;
	} else final.max = r.max;


	//On supprime les intervalles "avalés"
	_ranges.erase(_ranges.begin() + start.idx+1, _ranges.begin() + end.idx);

	recalculateSize();

	return true;
}

bool Domain::add(int n) {
	Domain::Location loc = locate(n);
	if(loc.inside)
		return false;

	bool prev = touchPrev(loc, n);
	bool next = touchNext(loc, n);
	int i = loc.idx;

	if(prev && next) {
		_ranges[i].max = _ranges[i+1].max;
		_ranges.erase(_ranges.begin() + i+1);
	} else if(prev) {
		_ranges[i].max++;
	} else if(next) {
		_ranges[i].min--;
	} else {
		_ranges.insert(_ranges.begin() + i, Range::from(n));
	}

	_size++;
	return true;
}

bool Domain::remove(int n) {
	Domain::Location loc = locate(n);
	if(!loc.inside)
		return false;

	Range& cur = _ranges[loc.idx];

	if(cur.isSingleton()) {
		_ranges.erase(_ranges.begin() + loc.idx);
	} else if(cur.min == n) {
		cur.min++;
	} else if(cur.max == n) {
		cur.max--;
	} else {
		Range r = Range {n+1, cur.max};
		cur.max = n-1;
		_ranges.insert(_ranges.begin() + loc.idx+1, r);
	}

	_size--;
	return true;
}

bool Domain::removeLessThan(int n, bool inclusive) {
	if(inclusive) {
		if(n == INT_MAX)
			return clear();
		n++;
	}

	int removed = 0;

	Domain::Location loc = locate(n);
	if(loc.inside) {
		removed = n - _ranges[loc.idx].min;
		_ranges[loc.idx].min = n;
	}
	else if(loc.idx == 0)
		return false;

	for(unsigned int i = 0; i < loc.idx; i++)
		removed += _ranges[i].size();

	_size -= removed;
	_ranges.erase(_ranges.begin(), _ranges.begin() + loc.idx);
	return true;
}


bool Domain::removeGreaterThan(int n, bool inclusive) {
	if(inclusive) {
		if(n == INT_MIN)
			return clear();
		n--;
	}

	int removed = 0;

	Domain::Location loc = locate(n);
	if(loc.inside) {
		removed = n - _ranges[loc.idx].max;
		_ranges[loc.idx].max = n;
	}
	else if(loc.idx == _ranges.size()-1)
		return false;

	for(unsigned int i = loc.idx; i < _ranges.size(); i++)
		removed += _ranges[i].size();

	_size -= removed;
	_ranges.erase(_ranges.begin() + loc.idx+1, _ranges.end());
	return true;
}

bool Domain::restrictTo(Range r) {
	if(r.isEmpty())
		return clear();

	bool modif = removeGreaterThan(r.max);
	modif |= removeLessThan(r.min);
	return modif;
}

bool Domain::restrictTo(Domain& dom) {
	if(dom.isEmpty())
		return clear();

	bool modif = restrictTo(dom.getEnclosingRange());
	if(dom._ranges.size() == 1)
		return modif;

	//TODO boucle compliquée
	/*
		this  =======	
		dom  ===  =====

	*/
	auto cur = _ranges.begin();
	auto other = dom._ranges.begin();

	while(true) {


	}

	recalculateSize();

	return modif;
}

std::ostream& operator<<(std::ostream& o, const Domain& d) {
	o << "{";

	bool first = true;
	for(auto& r: d._ranges) {
		if(first)
			first = false;
		else o << ", ";
		o << r;
	}

	return o << "}";
}

unsigned int Domain::getSize() const {

    unsigned int count = 0;
    for (unsigned int i = 0; i < _ranges.size(); i++) {
        count += _ranges[i].size();
    }

	return count;
}


Domain::iterator::iterator(std::vector<Range>::const_iterator it, int n):
	_cur(it),
	_n(n) {	
}

Domain::iterator& Domain::iterator::operator++() { // pre-increment

	if(_n < _cur->max) {
		_n++;
		return *this;
	}
	_cur++;

	return *this;
}

Domain::iterator Domain::iterator::operator++(int) { // post-increment
	Domain::iterator old = *this;
	++(*this);
	return old;
}


bool Domain::iterator::operator==(const Domain::iterator& other) const {
	//std::cout << ";" << *_cur << ";" <<  _n << " - " << *other._cur << ";" << other._n << std::endl;
	return other._cur == _cur && other._n == _n;
}

bool Domain::iterator::operator!=(const Domain::iterator& other) const {

	return !((*this) == other);
}

int Domain::iterator::operator*() {
	return _n;
}
int Domain::iterator::operator->() {
	return _n;
}

