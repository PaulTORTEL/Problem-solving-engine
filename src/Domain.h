#pragma once

#include <iostream>
#include <vector>

struct Range {
	int min; //inclusif
	int max; //inclusif

	//Renvoie true si n est dans l'intervalle
	bool contains(int n) const;

	//Renvoie true si l'intervalle est vide
	bool isEmpty() const;
	//Renvoie true si l'intervalle ne contient qu'un seul nombre
	bool isSingleton() const;
	//Renvoie le nombre de valeurs dans l'intervalle
	unsigned int size() const;

	//Construit un intervalle singleton {n}
	static Range from(int n);

	//Construit [min, max[
	static Range fromExclusive(int min, int max);
	//Construit [min, max]
	static Range fromInclusive(int min, int max);

	//Construit ]..., max[
	static Range lessThan(int max);
	//Construit ]..., max]
	static Range lessThanOrEqualsTo(int max);

	//Construit ]min, ...[
	static Range greaterThan(int max);
	//Construit [min, ...[
	static Range greaterThanOrEqualsTo(int max);
};

std::ostream& operator<<(std::ostream& o, const Range& r);


class Domain {
	//Liste des intervalles du domaine, avec ces propriétés:
	// - les intervalles sont par ordre croissant
	// - aucun intervalle n'est vide
	// - aucune paire d'intervalle ne se chevauche ni se touche
	std::vector<Range> _ranges;
	//Nombre de valeurs dans le domaine
	unsigned int _size;

public:
	class iterator {
		std::vector<Range>::const_iterator _cur;
		int _n;

    	iterator(std::vector<Range>::const_iterator it, int n);

	public:
		friend Domain;

    	iterator& operator++();   // pre-increment
    	iterator operator++(int); // post-increment

    	bool operator==(const iterator& other) const;
		bool operator!=(const iterator& other) const;

    	int operator*();
    	int operator->();

        int getValue(); // A vérifier par Arthur
    };


	//Construit un nouveau domaine vide
	Domain(){}

	//Renvoie la valeur minimale contenue dans le domaine
	//(non défini si le domaine est vide)
	int getMin() const;
	//Renvoie la valeur maximale contenue dans le domaine
	//(non défini si le domaine est vide)
	int getMax() const;

	//Renvoie le nombre de valeurs contenues dans le domaine
	unsigned int getSize() const;

	//Renvoie un itérateur sur les valeurs du domaine
	iterator begin() const;
	iterator end() const;

	//Renvoie un intervalle contenant toutes les valeurs du domaine
	//(non défini si le domaine est vide)
	Range getEnclosingRange() const;

	//Renvoie true si l'intervalle est vide
	bool isEmpty() const;
	//Renvoie true si l'intervalle ne contient qu'une seule valeur
	bool isSingleton() const;

	//Renvoie true si la valeur est contenue dans le domaine
	bool contains(int n) const;

	//Vide le domaine de toutes ses valeurs
	bool clear();

	//Ajoute un intervalle au domaine; renvoie true si le domaine a été modifié
	bool add(Range r);

	//Ajoute un nombre au domaine; renvoie true si le domaine a été modifié
	bool add(int n);

	//Supprime un nombre du domaine; renvoie true si le domaine a été modifié
	bool remove(int n);

	//Supprime tout les nombres inférieurs à n du domaine;
	//renvoie true si le domaine a été modifié
	bool removeLessThan(int n, bool inclusive = false);

	//Supprime tout les nombres supérieurs à n du domaine;
	//renvoie true si le domaine a été modifié
	bool removeGreaterThan(int n, bool inclusive = false);

	//Supprime tous les nombres qui ne sont pas dans l'intervalle;
	//renvoie true si le domaine a été modifié
	bool restrictTo(Range r);

	//Supprime tous les nombres qui ne sont pas dans le domaine passé en paramètre;
	//renvoie true si le domaine a été modifié
	bool restrictTo(Domain& dom);

	friend std::ostream& operator<<(std::ostream& o, const Domain& r);

    int operator[] (int x);

private:
	//Une position dans la liste d'intervalle
	// - si inside == true, alors n est dans l'intervalle n°index
	// - si inside == false, alors n est juste à gauche de l'intervalle n°index
	struct Location {
		unsigned int idx;
		bool inside;
	};

	bool touchNext(Location loc, int n);
	bool touchPrev(Location loc, int n);

	void recalculateSize();

	//Calcule la position de n dans la liste d'intervalles
	Location locate(int n) const;

	//Calcule la position de n dans la liste d'intervalles,
	//en supposant que n est à droite de start
	Location locate(int n, Location start) const;
};


std::ostream& operator<<(std::ostream& o, const Domain& r);
