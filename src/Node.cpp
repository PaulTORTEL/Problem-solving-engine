#include "Node.h"

Node::Node(int index)
{
    _index = index;
}

Node::~Node()
{

}

bool Node::createDumbNode(int value, std::vector<Variable>& vars, std::vector<int>& chosenValues) {

    if ((unsigned)_index + 1 >= vars.size()) {
        // TEST CONTRAINTES
        // APPEL DE LA FONCTION QUI MEMORISE LA REPONSE SI OK

        return false;// ou true si c'est ok
    }


    Domain& d = vars[_index+1].getDomain();

    std::vector<int> values = d.getPossibleValues();

    for (unsigned int i = 0; i < values.size(); i++) {
        this->addChild(new Node(_index+1));
        chosenValues.push_back(values[i]);
        if (this->createDumbNode(values[i], vars, chosenValues)) {
            return true;
        }
        else {
            chosenValues.pop_back();
        }
    }


    return false;
}

void Node::addChild(Node * new_node) {
    _childs.push_back(new_node);
}
