#include "Engine.h"
#include "tinyxml.h"
#include "XMLUtil.h"
#include "Utils.h"


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

        TiXmlAttribute* typeAttr = findAttribute(varElem,"type");

        if (!typeAttr)
        {
            std::cerr << "Erreur, une variable doit avoir un type ! Utilisez l'attribut type ( variable " << nameAttr->Value() << " )" << std::endl;
            return false;
        }

        std::vector<TiXmlElement*> domain = findNodes(varElem,"Domain");

        if (domain.size() != 1)
        {
            std::cerr << "Erreur, une variable doit avoir une et une seule fois la balise <Domain> pour la variable " << nameAttr->Value() << std::endl;
            return false;
        }

        std::vector<TiXmlElement*> subdomains = findNodes(domain[0],"Subdomain");

        if (subdomains.size() < 1)
        {
            std::cerr << "Erreur, vous devez definir au moins un <Subdomain> pour la variable " << nameAttr->Value() << std::endl;
            return false;
        }

        std::map<int,VariableDomain*> vDomains;

        for (unsigned int i = 0; i < subdomains.size(); i++)
        {
            TiXmlElement* eDomain = subdomains[i];

            TiXmlAttribute* orderAttr = findAttribute(eDomain,"order");
            if (!orderAttr)
            {
                std::cerr << "Erreur, un domaine doit avoir un ordre ! Utilisez l'attribut order. Variable = " << nameAttr->Value() << std::endl;
                return false;
            }

            int order = 0;
            if (!parseNumber(orderAttr->Value(), &order))
            {
                std::cerr << "Erreur, l'ordre d'un sous domaine doit etre un nombre. Variable = " << nameAttr->Value() << std::endl;
                return false;
            }

            VariableDomain *vDomain = new VariableDomain(order);

            vDomains[order] = vDomain;


            TiXmlAttribute *domainAttr = findAttribute(eDomain,"domain");

            if (!domainAttr)
            {
                std::cerr << "Erreur, le domaine doit etre defini ! Utilisez l'attribut domain. Variable = " << nameAttr->Value() << std::endl;
                return false;
            }

            std::string sDomain = domainAttr->Value();

            std::vector<std::string> splitted = split(sDomain,' ');

            if (splitted.size() == 0)
            {
                std::cerr << "Erreur, le domaine doit au moins avoir un sous domaine. Variable = " << nameAttr->Value() << std::endl;
                return false;
            }

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

                Domain *d = new Domain(nMin, nMax);

                vDomain->AddSubDomain(d);

            }

        }

        for (unsigned int n = 1; n < vDomains.size() + 1; n++)
        {
            if(  vDomains.find( n ) == vDomains.end() )
            {
                std::cerr << "Erreur, les ordres de domaines ne sont pas corrects. Variable = " << nameAttr->Value() << std::endl;
                return false;
            }
        }



    }



    return false;
}
