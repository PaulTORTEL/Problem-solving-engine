#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include "xml/tinyxml.h"
#include "Variable.h"
#include "Node.h"
#include "Constraints.h"

class EngineCreationException: public std::runtime_error {
public:
	EngineCreationException(const std::string& msg):
		std::runtime_error(msg) {
	}
};

class Engine {

    std::vector<Variable> _variables;

    Constraints _constraints;
    Node *_root;


public:

    Engine(std::vector<Variable>&& variables);

    Constraints& getConstraints();
    const Constraints& getConstraints() const;

    std::vector<Variable>::iterator beginVars();
    std::vector<Variable>::iterator endVars();

    void createTree(int choice);
    static int getIndexByLevel(const std::vector<Variable>& variables, int level);

    void refreshVarsLevels(); // Remet tous les niveaux des variables à -1

};
