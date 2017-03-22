#include "Node.h"
#include <stdlib.h>
Node::Node(int index)
{
    _index = index;
    _count++;
}

Node::~Node()
{

}

unsigned int Node::_count = 0;

bool Node::createNode(int value, std::vector<Variable> vars, std::vector<int>& chosenValues, Constraints* constraints) {

   // std::cout << "X[" << chosenValues.size() << "]" << std::endl;
    // réduire le domaine du noeud actuel en un singleton
    Domain& domain_current_var = vars[_index].getDomain();
    domain_current_var.removeLessThan(chosenValues[chosenValues.size()-1]);
    domain_current_var.removeGreaterThan(chosenValues[chosenValues.size()-1]);

 //   std::cout << domain_current_var << "et value : " << chosenValues[chosenValues.size()-1] << std::endl;

    //std::cout << "value : " << value << " et l'index : " << _index << " et chosen " << chosenValues.size() << std::endl;
    //system("pause");
    if ((unsigned)_index + 1 >= vars.size()) {
        //std::cout << "on est passe dans la boucle " << std::endl;
        std::vector<Domain> all_vars_domain;
        for (Variable& var: vars) {
            Domain& dom = var.getDomain();

            //std::cout << var.getName() << " et " << dom << std::endl;
            all_vars_domain.push_back(dom); // On récupère tous les domaines
        }

        for (unsigned int i = 0; i < chosenValues.size(); i++) {
            if (!constraints->isValuePossible(all_vars_domain, i, chosenValues[i])) {
               // std::cout << "i : "<< vars[i].getName() << " et sa value : " << chosenValues[i] << std::endl;
//system("pause");
                return false;
            }
            else {
                //std::cout << "isValuePossible a renvoye true " << std::endl;
            }

        }


        // TEST CONTRAINTES
        // APPEL DE LA FONCTION QUI MEMORISE LA REPONSE SI OK

        return true;// ou true si c'est ok
    }

    //intervention de la Brigade civile

    if (!reduceDomains(chosenValues, vars, constraints)) // Si on a trouvé un ou plusieurs domaines de valeurs vides
        return false;

    //intervention de la Brigade anti-espions
    std::vector<int> already_treated;

    /*if (!edgeConsistency(vars, constraints, _index, already_treated))
        return false;*/

    Domain& d = vars[_index+1].getDomain();

    std::vector<int> values = d.getValues();

    for (int value : values) {

        Node * new_node = new Node(_index+1);
        this->addChild(new_node);
        chosenValues.push_back(value);

        if (new_node->createNode(value, vars, chosenValues, constraints)) {
            return true;
        }
        else {
            chosenValues.pop_back();
            delete(new_node);
        }
    }


    return false;
}

bool Node::reduceDomains(std::vector<int> const & chosenValues, std::vector<Variable>& vars, Constraints* constraints) {

    std::cout << "chosenVal : ";
    for (int c : chosenValues)
        std::cout << c << " " ;

    std::cout << std::endl << " taille du chosen: " << chosenValues.size() << std:: endl;
    if ((unsigned)_index + 1 >= vars.size())
        return true;

    std::vector<Domain> all_vars_domain;
    for (Variable& var: vars) {
        Domain& dom = var.getDomain();

        all_vars_domain.push_back(dom); // On récupère tous les domaines
    }

    for (unsigned int i = _index + 1; i < vars.size(); i++) {
        Domain& d = vars[i].getDomain();

        std::vector<int> values = d.getValues();
        for (unsigned int j = 0 ; j < values.size(); j++) {

            if (!constraints->isValuePossible(all_vars_domain, i, values[j])) {
                d.remove(values[j]);
            }
        }

        if (d.isEmpty())  // 1 ou plusieurs domaines vides
            return false;

    }

    return true;
}

void Node::addChild(Node * new_node) { // X1 > X2      X1 < X3          X2 > X4
    _children.push_back(new_node);
}

bool Node::edgeConsistency(std::vector<Variable>& vars, Constraints* constraints, unsigned int index, std::vector<int>& already_treated) {

    std::cout << "on va push : " << index << std::endl;
    already_treated.push_back(index);

    std::vector<int> related_var_index = constraints->getRelatedVariablesIndex(index);

    // épurage des index déjà traités mais retrouvés de par l'aspect miroir du tableau de contraintes
    // (X1 > X2    X2 < X3    X3 < X1 ==> on retrouve X1 à partir de X3, alors qu'on l'a déjà traité

    for (int i : related_var_index) {
        std::cout << i+1 << std::endl;
        for (int j : already_treated) {
            if (i == j) {
                    std::cout << "putain de bordel i : " << i+1 << std::endl;
                related_var_index.erase(related_var_index.begin() + i);
            }
        }
    }
std::cout << std::endl;
    for (int i : related_var_index) {
        if ((unsigned) i < index)
            related_var_index.erase(related_var_index.begin() + i);
        std::cout <<  i+1 << std::endl;
    }

    //system("pause");


    std::vector<Domain> all_vars_domain;
        for (Variable& var: vars) {
            Domain& dom = var.getDomain();
            all_vars_domain.push_back(dom); // On récupère tous les domaines
    }

    for (unsigned int i = 0 ; i < related_var_index.size(); i++) {
std::cout << " omg so tard X" << related_var_index[i] << std::endl;
        Domain& d = vars[related_var_index[i]].getDomain();
        std::vector<int> values = d.getValues();

        for (unsigned int j = 0 ; j < values.size(); j++) {
            std::cout << "related_var_index[i]+1: " << related_var_index[i]+1 << std::endl;
           // system("pause");
            if (!constraints->isValuePossible(all_vars_domain, related_var_index[i], values[j])) {
                std::cout << values[j] << " vient d etre suppr du domaine de X" << related_var_index[i]+1 << " et d : " << d << std::endl;
                d.remove(values[j]);
            }
            else {
                std::cout << "\t wallah frer ca marche qued " << std::endl;
                if (!this->edgeConsistency(vars, constraints, related_var_index[i], already_treated)) {
                    std::cout << "\t on depush " << already_treated.back();
                    already_treated.pop_back();
                    return false;
                }
            }
        }

        std::cout << "le domaine de X" << related_var_index[i] + 1 << " => " << d << std::endl << std::endl << std::endl;

        if (d.isEmpty()){  // 1 ou plusieurs domaines vides
            std::cout << "\t on depush " << already_treated.back();
            return false;
        }
    }

    return true;
}
