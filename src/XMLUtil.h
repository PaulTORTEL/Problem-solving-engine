#pragma once

#include "tinyxml.h"
#include <iostream>
#include <vector>

std::vector<TiXmlElement*> findNodes(TiXmlElement* root, const char* name);
TiXmlAttribute* findAttribute(TiXmlElement* root, const char* name);

