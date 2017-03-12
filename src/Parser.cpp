#include "Engine.h"
#include "tinyxml.h"
#include "XMLUtil.h"
#include "Utils.h"
#include "Domain.h"
#include "Indexer.h"

bool Engine::readVariables()
{

    TiXmlDocument doc(_fileName.c_str());

    if(!doc.LoadFile())
    {
        std::cerr << "erreur lors du chargement" << std::endl;
        std::cerr << "error #" << doc.ErrorId() << " : " << doc.ErrorDesc() << std::endl;
        return false;
    }


    TiXmlHandle hdl(&doc);


    TiXmlElement *root = hdl.FirstChildElement().Element();

    std::vector<TiXmlElement*> all_var = findNodes(root,"Vars");

    if (all_var.size() != 1)
    {
        std::cerr << "Erreur, vous avez mal saisi les variables !" << std::endl;
        return false;
    }

    std::vector<TiXmlElement*> vars = findNodes(all_var[0],"Var");

    for (unsigned int k = 0; k < vars.size(); k++)
    {
        TiXmlElement* varElem = vars[k];

        TiXmlAttribute* nameAttr = findAttribute(varElem,"name");
        if (!nameAttr)
        {
            std::cerr << "Erreur, une variable doit avoir un nom ! Utilisez l'attribut name" << std::endl;
            return false;
        }

        std::vector<TiXmlElement*> domains = findNodes(varElem,"Domain");

        if (domains.size() != 1)
        {
            std::cerr << "Erreur,il doit y avoir un domaine, utilisez <Domain> pour la variable  " << nameAttr->Value() << std::endl;
            return false;
        }

        TiXmlElement* domain = domains[0];

        TiXmlAttribute* domainValue = findAttribute(domain,"value");

        if (!domainValue)
        {
            std::cerr << "Erreur, un domain doit avoir un nom ! Utilisez l'attribut value (variable = " << nameAttr->Value() << ")" << std::endl;
            return false;
        }


        std::string sDomain = domainValue->Value();

        std::vector<std::string> splitted = split(sDomain,' ');

        if (splitted.size() == 0)
        {
            std::cerr << "Erreur, le domaine doit au moins avoir un sous domaine. Variable = " << nameAttr->Value() << std::endl;
            return false;
        }


        Domain dom;


        for (unsigned int j = 0; j < splitted.size(); j++)
        {
            std::vector<std::string> interv = split(splitted[j],';');
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
            else
            {
                std::cerr << "Erreur, un intervalle doit etre de la forme i;j ou i. Variable = " << nameAttr->Value() << std::endl;
                return false;
            }

            int nMin = 0;
            int nMax = 0;

            if (!parseNumber(sMin.c_str(),&nMin) || !parseNumber(sMax.c_str(),&nMax))
            {
                std::cerr << "Erreur, les intervalles doivent etre composes de nombre entier. Variable = " << nameAttr->Value() << std::endl;
                return false;
            }


            Range r = Range::fromInclusive(nMin,nMax);
            dom.add(r);
        }

        std::vector<TiXmlElement*> indexes = findNodes(varElem,"Index");

        std::string name(nameAttr->Value());

        if (indexes.size() == 0)
        {

            Variable variable(dom,name);

            std::cout << variable << std::endl;

            //TODO: PushBack
            _variables.push_back(variable); // ?
        }

        else
        {

            Indexer indexer;
            for (unsigned int i = 0; i < indexes.size(); i++)
            {
                TiXmlElement* elemIndex = indexes[i];
                TiXmlAttribute* indexAttr = findAttribute(elemIndex,"max");
                if (!indexAttr)
                {
                    std::cerr << "Erreur, un index doit avoir l'attribut max. Variable = " << nameAttr->Value() << std::endl;
                    return false;
                }

                int indexMax = 0;
                if (!parseNumber(indexAttr->Value(),&indexMax))
                {
                    std::cerr << "Erreur, un index doit avoir une valeur max entiere. Variable = " << nameAttr->Value() << std::endl;
                    return false;
                }

                indexer.addIndex(indexMax);

            }
            int cpt = 1;

            std::string temp;

            while (indexer.hasNext())
            {

                cpt++;
                temp = indexer.getVarIndex();
                indexer.next();

                temp = concatString(name,temp);
                Variable v(dom,temp);
                std::cout << v << std::endl;
                _variables.push_back(v);
            }

            temp = indexer.getVarIndex();

            temp = concatString(name,temp);

            Variable v(dom,temp);

            std::cout << v << std::endl;
            _variables.push_back(v);
        }

        //TODO: Ajouter les variables quelque part (et pas oublier de register l'index)

    }

    return false;

}
