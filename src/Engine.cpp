#include "Engine.h"
#include "Node.h"

Engine::Engine() {
}

void Engine::createDumbTree(int index) {

    //On crée le premier noeud (qui sert uniquement de point de jonction donc value inutile et index inutile)
    _root = new Node(index);

    Domain& d = _variables[0].getDomain();

    std::vector<int> values = d.getPossibleValues();

    for (unsigned int i = 0; i < values.size(); i++) {
        Node *new_node = new Node(index+1);
        _root->addChild(new_node);
        std::vector<int> chosenValues;
        chosenValues.push_back(values[i]);

        if (new_node->createDumbNode(values[i], _variables, chosenValues, _constraints)) {
            // C'est ok il faut enregistrer la réponse (chosenValues à afficher)
            std::cout << "c'est ok " << std::endl;

            for (unsigned int j = 0; j < chosenValues.size(); j++)
                std::cout << "Pour var " << j << " value => " << chosenValues[j] << std::endl;
            break;
        }
    }

}

void Engine::tempConstFulfiller() {

    _constraints = new Constraints(_variables.size());

    /*_constraints->addBinConstraint(1, 2, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);
    _constraints->addBinConstraint(1, 3, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);*/
    _constraints->addBinConstraint(1, 0, /*Constraints::BIN_CON_LESS | */Constraints::BIN_CON_GREATER);

    /*_constraints->addBinConstraint(2, 1, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);
    _constraints->addBinConstraint(2, 3, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);*/
    _constraints->addBinConstraint(2, 1, /*Constraints::BIN_CON_LESS | */Constraints::BIN_CON_GREATER);

    /*_constraints->addBinConstraint(3, 1, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);
    _constraints->addBinConstraint(3, 2, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);*/
    _constraints->addBinConstraint(3, 2, /*Constraints::BIN_CON_LESS |*/ Constraints::BIN_CON_GREATER);

    /*_constraints->addBinConstraint(0, 1, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);
    _constraints->addBinConstraint(0, 2, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);
    _constraints->addBinConstraint(0, 3, Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER);*/
}
