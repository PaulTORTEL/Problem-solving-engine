#pragma once

#include <stdlib.h>
#include "Engine.h"
#include "xml/tinyxml.h"
#include <map>

class XMLParser {


public:
    //Peut lancer une EngineCreationException
	static Engine fromFile(const std::string& file);

    //Peut lancer une EngineCreationException
	static Engine from(const TiXmlHandle& hdl);

};
