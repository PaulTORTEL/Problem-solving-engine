#include "XMLUtil.h"

std::vector<TiXmlElement*> findNodes(TiXmlElement* root, const char* name) {

    std::vector<TiXmlElement*> nodes;

    TiXmlElement* elem = root->FirstChildElement();

    while (elem) {
        if (!strcmp(name,elem->Value())) {
            nodes.push_back(elem);
        }
        elem = elem->NextSiblingElement();
    }

    return nodes;
}

TiXmlAttribute* findAttribute(TiXmlElement* root, const char* name) {


    TiXmlAttribute* attr = root->FirstAttribute();

    while (attr) {
        if (!strcmp(name,attr->Name())) {
            return attr;
        }
        attr = attr->Next();
    }

    return NULL;
}
