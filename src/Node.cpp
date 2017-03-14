#include "Node.h"
#include <stdlib.h>
Node::Node(int index)
{
    _index = index;
}

Node::~Node()
{

}

bool Node::createDumbNode(int value, std::vector<Variable> vars, std::vector<int>& chosenValues, Constraints* constraints) {

    std::cout << "value : " << value << " et l'index : " << _index << " et chosen " << chosenValues.size() << std::endl;
    //system("pause");
    if ((unsigned)_index + 1 >= vars.size()) {

        std::vector<Domain> all_vars_domain;
        for (Variable& var: vars) {
            Domain& dom = var.getDomain();
            std::cout << var.getName() << " et " << dom << std::endl;
            all_vars_domain.push_back(dom); // On récupère tous les domaines
        }

        for (int i = vars.size() - 1; i >= 0; i--) {
            if (!constraints->isValuePossible(all_vars_domain, i, chosenValues[i])) {
                std::cout << "i : "<< vars[i].getName() << " et sa value : " << chosenValues[i] << std::endl;
//system("pause");
                return false;
            }
            else {
                std::cout << "isValuePossible a renvoye true " << std::endl;
            }

        }


        // TEST CONTRAINTES
        // APPEL DE LA FONCTION QUI MEMORISE LA REPONSE SI OK

        return true;// ou true si c'est ok
    }


    Domain& d = vars[_index+1].getDomain();
    for (int value: chosenValues)
        d.remove(value);

    std::vector<int> values = d.getValues();
    std::cout << "values : " << values[0] << std::endl;
   // system("pause");
    for (int value: values) {
        Node * new_node = new Node(_index+1);
        this->addChild(new_node);
        chosenValues.push_back(value);
        if (new_node->createDumbNode(value, vars, chosenValues, constraints)) {
            return true;
        }
        else {
            chosenValues.pop_back();
        }
    }


    return false;
}

void Node::addChild(Node * new_node) {
    _children.push_back(new_node);
}
