#ifndef XMLUTIL_H_INCLUDED
#define XMLUTIL_H_INCLUDED

#include "tinyxml.h"
#include <iostream>
#include <vector>

std::vector<TiXmlElement*> findNodes(TiXmlElement* root, const char* name);
TiXmlAttribute* findAttribute(TiXmlElement* root, const char* name);


#endif // XMLUTIL_H_INCLUDED
