#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include "tinyxml.h"
#include "Variable.h"

class EngineCreationException: public std::runtime_error {
public:
	EngineCreationException(const std::string& msg):
		std::runtime_error(msg) {
	}
};

class Engine {
    Engine();

    std::vector<Variable> _variables;

  	void readVariablesFromXML(const TiXmlHandle& hdl);

public:
	//Peut lancer une EngineCreationException
	static Engine fromXMLFile(const std::string& file);


    


};
