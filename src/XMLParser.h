#pragma once

#include <stdlib.h>
#include "Engine.h"
#include "xml/tinyxml.h"

class XMLParser {
	XMLParser();

public:

    //Peut lancer une EngineCreationException
	static Engine fromFile(const std::string& file);

    //Peut lancer une EngineCreationException
	static Engine from(const TiXmlHandle& hdl);

};