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
    Engine();

    std::vector<Variable> _variables;

    Constraints *_constraints;
    Node *_root;

    void readVariablesFromXML(const TiXmlHandle& hdl);

    public:
        Engine(const std::string& fileName);

        void createDumbTree(int index);

        //Peut lancer une EngineCreationException
        static Engine fromXMLFile(const std::string& file);
        void tempConstFulfiller();

};
