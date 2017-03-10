
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
	return _ranges.empty() ? INT_MIN : _ranges.back().min;
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
		return true;
	}

	Domain::Location end = locate(r.max, start);

	//On ajuste r pour pas que ça touche les intervalles
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
			return true;
		}

		//Contenu dans un intervalle, on fait rien
		if(start.inside && end.inside)
			return false;
	}

	Range& final = _ranges[start.idx];

	//On ajuste la borne inférieure
	if(!start.inside)
		final.min = r.min;

	//On ajuste la borne supérieure
	if(end.inside) {
		final.max = _ranges[end.idx].max;
		end.idx++;
	} else final.max = r.max;


	//On supprime les intervalles "avalés"
	_ranges.erase(_ranges.begin() + start.idx+1, _ranges.begin() + end.idx);

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

	return true;
}

bool Domain::removeLessThan(int n, bool inclusive) {
	if(inclusive) {
		if(n == INT_MAX) {
			bool empty = _ranges.empty();
			_ranges.clear();
			return empty;
		}
		n++;
	}

	Domain::Location loc = locate(n);
	if(loc.inside)
		_ranges[loc.idx].min = n;
	else if(loc.idx == 0)
		return false;

	_ranges.erase(_ranges.begin(), _ranges.begin() + loc.idx);
	return true;
}


bool Domain::removeGreaterThan(int n, bool inclusive) {
	if(inclusive) {
		if(n == INT_MIN) {
			bool empty = _ranges.empty();
			_ranges.clear();
			return empty;
		}
		n--;
	}

	Domain::Location loc = locate(n);
	if(loc.inside)
		_ranges[loc.idx].max = n;
	else if(loc.idx == _ranges.size()-1)
		return false;

	_ranges.erase(_ranges.begin() + loc.idx+1, _ranges.end());
	return true;
}

bool Domain::restrictTo(Range r) {
	if(r.isEmpty()) {
		bool empty = _ranges.empty();
		_ranges.clear();
		return empty;
	}

	bool modif = removeLessThan(r.min);
	modif |= removeGreaterThan(r.max);
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