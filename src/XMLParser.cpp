
#include "XMLParser.h"

#include "XMLUtil.h"
#include "Utils.h"
#include "Domain.h"
#include "Indexer.h"

static std::vector<Variable> readVariables(const TiXmlHandle& hdl);

static Domain readDomain(std::stringstream& err, const std::string& str);


Engine XMLParser::fromFile(const std::string& file) {
    TiXmlDocument doc(file.c_str());

    if(!doc.LoadFile()) {
        std::stringstream err;
        err << "Impossible de lire le fichier " << file << " : " << std::endl;
        err << "error #" << doc.ErrorId() << " : " << doc.ErrorDesc();
        throw EngineCreationException(err.str());
    }

    TiXmlHandle hdl(&doc);

    return XMLParser::from(hdl);
}

Engine XMLParser::from(const TiXmlHandle& hdl) {
    Engine engine(readVariables(hdl));

    return engine;

}


static Domain readDomain(std::stringstream& err, const std::string& sDomain) {

    std::vector<std::string> splitted = split(sDomain,' ');

    if (splitted.size() == 0) {
        err << "Le domaine ne peut etre vide !";
        throw EngineCreationException(err.str());
    }


    Domain dom;

    for (const std::string& s: splitted)
    {
        std::vector<std::string> interv = split(s,';');
        std::string sMin = "";
        std::string sMax = "";

        if (interv.size() == 1)
        {
            sMin = interv[0];
            sMax = interv[0];
        }
        else if (interv.size() == 2)
        {
            sMin = interv[0];
            sMax = interv[1];
        }
        else {
            err << "Un intervalle doit etre de la forme i;j ou i !";
            throw EngineCreationException(err.str());
        }
        int nMin = 0;
        int nMax = 0;

        if (!parseNumber(sMin.c_str(),&nMin) || !parseNumber(sMax.c_str(),&nMax)) {
            err << "Les intervalles doivent etre composes de nombres entiers !";
            throw EngineCreationException(err.str());
        }


        Range r = Range::fromInclusive(nMin,nMax);
        dom.add(r);
    }

    return dom;
}

static std::vector<Variable> readVariables(const TiXmlHandle& hdl) {
    std::stringstream err;

    TiXmlElement *root = hdl.FirstChildElement().Element();

    std::vector<TiXmlElement*> all_var = findNodes(root,"Vars");

    if (all_var.size() != 1) {
        throw EngineCreationException("Il doit y avoir exactement un noeud Vars");
    }

    std::vector<TiXmlElement*> vars = findNodes(all_var[0],"Var");
    std::vector<Variable> variables;

    for (TiXmlElement* varElem: vars) {

        TiXmlAttribute* nameAttr = findAttribute(varElem,"name");
        if (!nameAttr) {
            throw EngineCreationException("Une variable doit avoir un nom ! Utilisez l'attribut name.");
        }

        err.str("");
        err << "[Variable " << nameAttr->Value() << "] ";

        std::vector<TiXmlElement*> domains = findNodes(varElem,"Domain");

        if (domains.size() != 1) {
            err << "La variable doit posseder un domaine ! Utilisez <Domain>.";
            throw EngineCreationException(err.str());
        }

        TiXmlElement* domain = domains[0];

        TiXmlAttribute* domainValue = findAttribute(domain,"value");

        if (!domainValue) {
            err << "Un domaine doit avoir une valeur ! Utilisez l'attribut value.";
            throw EngineCreationException(err.str());
        }


        Domain dom = readDomain(err, domainValue->Value());


        std::vector<TiXmlElement*> indexes = findNodes(varElem,"Index");

        std::string name(nameAttr->Value());

        Indexer indexer;
        for (unsigned int i = 0; i < indexes.size(); i++) {
            TiXmlElement* elemIndex = indexes[i];
            TiXmlAttribute* indexAttr = findAttribute(elemIndex,"max");
            if (!indexAttr) {
                err << "Un index doit avoir l'attribut max !";
                throw EngineCreationException(err.str());
            }

            int indexMax = 0;
            if (!parseNumber(indexAttr->Value(),&indexMax)) {
                err << "Un index doit avoir une valeur max entiere !";
                throw EngineCreationException(err.str());
            }

            indexer.addIndex(indexMax);

        }


        if(indexer.numIndices() == 0) {
            variables.emplace_back(Variable(dom, name));
        } else {

            while (indexer.hasNext()) {
                std::string index = indexer.next();
                variables.emplace_back(Variable(dom, name + index));
            }
        }
        //TODO: Ajouter les variables quelque part (et pas oublier de register l'index)

    }

    return variables;
}
