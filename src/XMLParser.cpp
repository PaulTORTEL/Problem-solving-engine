
#include "XMLParser.h"

#include "XMLUtil.h"
#include "Utils.h"
#include "Domain.h"
#include "Indexer.h"
#include <regex>
#include <string>

static std::vector<Variable> readVariables(const TiXmlHandle& hdl, std::map<std::string, int>& indexesMap);

static void readConstraints(Engine& engine, const TiXmlHandle& hdl);

static Domain readDomain(std::stringstream& err, const std::string& str);


static void loadIndexes(std::vector< std::vector <int> >& indexes, std::string indexStr);

static char GetOperator(std::string op);

static void ReduceVariable(Variable* var, int value, std::string op);

Engine XMLParser::fromFile(const std::string& file)
{
    TiXmlDocument doc(file.c_str());

    if(!doc.LoadFile())
    {
        std::stringstream err;
        err << "Impossible de lire le fichier " << file << " : " << std::endl;
        err << "error #" << doc.ErrorId() << " : " << doc.ErrorDesc();
        throw EngineCreationException(err.str());
    }

    TiXmlHandle hdl(&doc);

    return XMLParser::from(hdl);
}

Engine XMLParser::from(const TiXmlHandle& hdl)
{
    std::map<std::string, int> indexes;

    Engine engine(readVariables(hdl,indexes));
    engine.setIndexes(indexes);

    readConstraints(engine, hdl);
    return engine;

}


static Domain readDomain(std::stringstream& err, const std::string& sDomain)
{

    std::vector<std::string> splitted = split(sDomain,' ');

    if (splitted.size() == 0)
    {
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
        else
        {
            err << "Un intervalle doit etre de la forme i;j ou i !";
            throw EngineCreationException(err.str());
        }
        int nMin = 0;
        int nMax = 0;

        if (!parseNumber(sMin.c_str(),&nMin) || !parseNumber(sMax.c_str(),&nMax))
        {
            err << "Les intervalles doivent etre composes de nombres entiers !";
            throw EngineCreationException(err.str());
        }


        Range r = Range::fromInclusive(nMin,nMax);
        dom.add(r);
    }

    return dom;
}

static std::vector<Variable> readVariables(const TiXmlHandle& hdl, std::map<std::string, int>& indexesMap)
{
    std::stringstream err;

    TiXmlElement *root = hdl.FirstChildElement().Element();

    std::vector<TiXmlElement*> all_var = findNodes(root,"Vars");

    if (all_var.size() != 1)
    {
        throw EngineCreationException("Il doit y avoir exactement un noeud Vars");
    }

    std::vector<TiXmlElement*> vars = findNodes(all_var[0],"Var");
    std::vector<Variable> variables;

    for (TiXmlElement* varElem: vars)
    {

        TiXmlAttribute* nameAttr = findAttribute(varElem,"name");
        if (!nameAttr)
        {
            throw EngineCreationException("Une variable doit avoir un nom ! Utilisez l'attribut name.");
        }

        err.str("");
        err << "[Variable " << nameAttr->Value() << "] ";

        std::vector<TiXmlElement*> domains = findNodes(varElem,"Domain");

        if (domains.size() != 1)
        {
            err << "La variable doit posseder un domaine ! Utilisez <Domain>.";
            throw EngineCreationException(err.str());
        }

        TiXmlElement* domain = domains[0];

        TiXmlAttribute* domainValue = findAttribute(domain,"value");

        if (!domainValue)
        {
            err << "Un domaine doit avoir une valeur ! Utilisez l'attribut value.";
            throw EngineCreationException(err.str());
        }


        Domain dom = readDomain(err, domainValue->Value());


        std::vector<TiXmlElement*> indexes = findNodes(varElem,"Index");

        std::string name(nameAttr->Value());

        Indexer indexer;
        for (unsigned int i = 0; i < indexes.size(); i++)
        {
            TiXmlElement* elemIndex = indexes[i];
            TiXmlAttribute* indexAttr = findAttribute(elemIndex,"max");
            if (!indexAttr)
            {
                err << "Un index doit avoir l'attribut max !";
                throw EngineCreationException(err.str());
            }

            int indexMax = 0;
            if (!parseNumber(indexAttr->Value(),&indexMax))
            {
                err << "Un index doit avoir une valeur max entiere !";
                throw EngineCreationException(err.str());
            }

            indexer.addIndex(indexMax);

        }


        if(indexer.numIndices() == 0)
        {
            indexesMap[name] = variables.size();
            variables.emplace_back(Variable(dom, name));

        }
        else
        {

            while (indexer.hasNext())
            {
                std::string index = indexer.next();
                indexesMap[name + index] = variables.size();
                variables.emplace_back(Variable(dom, name + index));
            }
        }
        //TODO: Ajouter les variables quelque part (et pas oublier de register l'index)
    }

    return variables;
}

static void readConstraints(Engine& engine, const TiXmlHandle& hdl)
{
    std::stringstream err;

    TiXmlElement *root = hdl.FirstChildElement().Element();

    std::vector<TiXmlElement*> all_cstr = findNodes(root,"Constraints");

    if (all_cstr.size() != 1)
    {
        throw EngineCreationException("Il doit y avoir exactement un noeud Constraints");
    }

    std::vector<TiXmlElement*> cstrs = findNodes(all_cstr[0],"Constraint");

    Constraints& constraints = engine.getConstraints();

    for (TiXmlElement* cstrElem: cstrs)
    {
        TiXmlAttribute* typeAttr = findAttribute(cstrElem,"type");

        if (!typeAttr)
        {
            throw EngineCreationException("Une contrainte doit avoir un type! Utilisez l'attribut type.");
        }

        if (!strcmp(typeAttr->Value(),"bin"))
        {
            TiXmlAttribute* var1Attr = findAttribute(cstrElem,"var1");

            if (!var1Attr)
            {
                throw EngineCreationException("Une contrainte binnaire doit avoir une premiere variable. utilisez l'attribut var1.");
            }

            std::string var1 = var1Attr->Value();

            TiXmlAttribute* var2Attr = findAttribute(cstrElem,"var2");

            bool isValue = false;

            std::string var2 = "";

            int value = 0;

            if (!var2Attr)
            {
                TiXmlAttribute* valueAttr = findAttribute(cstrElem,"value");

                if (!valueAttr)
                {
                    throw EngineCreationException("Une contrainte binnaire doit avoir une deuxieme variable ou une valeur fixe. utilisez l'attribut var2 ou value.");
                }

                isValue = true;

                if (!parseNumber(valueAttr->Value(),&value))
                {
                    throw EngineCreationException("L'attribut value d'une contrainte binnaire doit etre un nombre entier");
                }
            }

            else
                var2 = var2Attr->Value();


            TiXmlAttribute* opAttr = findAttribute(cstrElem,"op");

            if (!opAttr)
            {
                throw EngineCreationException("Une contrainte binnaire doit avoir un operateur. Utilisez l'attribut op");
            }

            std::string opStr = opAttr->Value();

            char opChar = GetOperator(opStr);

            if (opChar == -1)
            {
                throw EngineCreationException("Les operateurs binnaires sont : =  !=  <  >  <=  >=");
            }

            std::regex validPattern { "[[:alnum:]]+(\\[[[:digit:]]+([;][[:digit:]]+)?\\])*" };
            std::regex searchPatternVar { "([[:alnum:]]+)(\\[[[:digit:]]+([;][[:digit:]]+)?\\])*" };
            std::regex searchPatternIndexes { "[[:alnum:]]+((\\[[[:digit:]]+([;][[:digit:]]+)?\\])*)" };

            if(!std::regex_match(var1, validPattern))
            {
                throw EngineCreationException("Mauvaise syntaxe de variable (var1)");
            }

            if(!isValue && !std::regex_match(var2, validPattern))
            {
                throw EngineCreationException("Mauvaise syntaxe de variable (var2)");
            }

            std::string var1Name = regex_replace(var1, searchPatternVar, "$1");
            std::string var1Indexes = regex_replace(var1, searchPatternIndexes, "$1");

            std::string var2Name = "TempVar";
            std::string var2Indexes = "";

            if (!isValue)
            {
                var2Name = regex_replace(var2, searchPatternVar, "$1");
                var2Indexes = regex_replace(var2, searchPatternIndexes, "$1");
            }

            std::vector< std::vector<int> > var1IndexesArray;
            std::vector< std::vector<int> > var2IndexesArray;

            loadIndexes(var1IndexesArray,var1Indexes);

            Indexer indexerVar1;
            Indexer indexerVar2;

            for (unsigned int i = 0; i < var1IndexesArray.size(); i++)
            {
                indexerVar1.addIndex(var1IndexesArray[i][1],var1IndexesArray[i][0]);
            }

            if (!isValue)
            {
                loadIndexes(var2IndexesArray,var2Indexes);

                for (unsigned int i = 0; i < var2IndexesArray.size(); i++)
                {
                    indexerVar2.addIndex(var2IndexesArray[i][1],var2IndexesArray[i][0]);
                }
            }


            if (indexerVar1.numIndices() != 0)
            {
                if (!isValue)
                {
                    while (indexerVar1.hasNext())
                    {
                        std::string dynVar1 = var1Name + indexerVar1.next();

                        int indexVar1 = engine.getIndex(dynVar1);

                        if (indexVar1 == -1)
                        {
                            err << "Cette variable n'existe pas : " << dynVar1;
                            throw EngineCreationException(err.str());
                        }

                        if (indexerVar2.numIndices() != 0)
                        {
                            while (indexerVar2.hasNext())
                            {

                                std::string dynVar2 = var2Name + indexerVar2.next();
                                int indexVar2 = engine.getIndex(dynVar2);

                                if (indexVar2 == -1)
                                {
                                    err << "Cette variable n'existe pas : " << dynVar2;
                                    throw EngineCreationException(err.str());
                                }

                                std::cout << dynVar1 << "(" << indexVar1 << ")" << opStr << dynVar2 << "(" << indexVar2 << ")" << std::endl;
                                constraints.addBinConstraint(indexVar1,indexVar2,opChar);
                            }

                            indexerVar2.reset();

                        }
                        else
                        {
                            int indexVar2 = engine.getIndex(var2Name);

                            if (indexVar2 == -1)
                            {
                                err << "Cette variable n'existe pas : " << var2Name;
                                throw EngineCreationException(err.str());
                            }

                            std::cout << dynVar1 << "(" << indexVar1 << ")" << opStr << var2Name << "(" << indexVar2 << ")" << std::endl;
                            constraints.addBinConstraint(indexVar1,indexVar2,opChar);
                        }

                    }

                }
                else
                {
                    while (indexerVar1.hasNext())
                    {
                        std::string dynVar1 = var1Name + indexerVar1.next();

                        int indexVar1 = engine.getIndex(dynVar1);

                        if (indexVar1 == -1)
                        {
                            err << "Cette variable n'existe pas : " << dynVar1;
                            throw EngineCreationException(err.str());
                        }

                        Variable* var = engine.getVariableByIndex(indexVar1);

                        ReduceVariable(var,value,opStr);
                    }
                }

            }

            else
            {

                if (!isValue)
                {
                    int indexVar1 = engine.getIndex(var1Name);

                    if (indexVar1 == -1)
                    {
                        err << "Cette variable n'existe pas : " << var1Name;
                        throw EngineCreationException(err.str());
                    }

                    if (indexerVar2.numIndices() != 0)
                    {
                        while (indexerVar2.hasNext())
                        {

                            std::string dynVar2 = var2Name + indexerVar2.next();
                            int indexVar2 = engine.getIndex(dynVar2);

                            if (indexVar2 == -1)
                            {
                                err << "Cette variable n'existe pas : " << dynVar2;
                                throw EngineCreationException(err.str());
                            }

                            std::cout << var1Name << "(" << indexVar1 << ")" << opStr << dynVar2 << "(" << indexVar2 << ")" << std::endl;
                            constraints.addBinConstraint(indexVar1,indexVar2,opChar);
                        }

                        indexerVar2.reset();

                    }
                    else
                    {
                        int indexVar2 = engine.getIndex(var2Name);

                        if (indexVar2 == -1)
                        {
                            err << "Cette variable n'existe pas : " << var2Name;
                            throw EngineCreationException(err.str());
                        }

                        std::cout << var1Name << "(" << indexVar1 << ")" << opStr << var2Name << "(" << indexVar2 << ")" << std::endl;
                        constraints.addBinConstraint(indexVar1,indexVar2,opChar);
                    }
                }
                else
                {
                    int indexVar1 = engine.getIndex(var1Name);

                    if (indexVar1 == -1)
                    {
                        err << "Cette variable n'existe pas : " << var1Name;
                        throw EngineCreationException(err.str());
                    }

                    Variable* var = engine.getVariableByIndex(indexVar1);

                    ReduceVariable(var,value,opStr);

                }
            }
        }
        else if (!strcmp(typeAttr->Value(),"sum"))
        {

            TiXmlAttribute* valueAttr = findAttribute(cstrElem,"value");
            TiXmlAttribute* refAttr = findAttribute(cstrElem,"ref");

            int value = 0;

            bool isValue = false;



            if (!valueAttr && !refAttr)
            {
                throw EngineCreationException("Une contrainte de somme doit avoir une valeur constante ou une variable. Utilisez l'attribut value ou ref");
            }

            if (valueAttr)
                isValue = true;


            std::string variableStr = "";
            Variable* variable = nullptr;
            int dex = -2;
            if (isValue)
            {
                if (!parseNumber(valueAttr->Value(),&value))
                {
                    throw EngineCreationException("L'attribut value d'une contrainte binnaire doit etre un nombre entier");
                }
            }
            else
            {
                variableStr = refAttr->Value();

                dex = engine.getIndex(variableStr);

                if (dex == -1)
                {
                    err << "L'attribut ref doit faire reference a une variable existante" << std::endl;
                    err << variableStr << " n'existe pas.";
                    throw EngineCreationException(err.str());
                }

                variable = engine.getVariableByIndex(dex);

            }

            TiXmlAttribute* opAttr = findAttribute(cstrElem,"op");

            if (!opAttr)
            {
                throw EngineCreationException("Une contrainte de somme doit avoir un operateur. Utilisez l'attribut op");
            }

            std::string opStr = opAttr->Value();

            std::vector<TiXmlElement*> sumVarsNodes = findNodes(cstrElem,"Var");

            if (sumVarsNodes.size() < 1)
            {
                throw EngineCreationException("Une contrainte de somme doit avoir au moins un noeud Var.");
            }

            std::vector<VarCoeff> varsCoeff;

            for (TiXmlElement* varElem: sumVarsNodes)
            {
                TiXmlAttribute* varAttr = findAttribute(varElem,"name");

                if (!varAttr)
                {
                    throw EngineCreationException("Une variable dans une contrainte de somme doit avoir un nom. Utilisez l'attribut name.");
                }

                std::string varStr = varAttr->Value();

                TiXmlAttribute* coeffAttr = findAttribute(varElem,"coeff");

                int coeff = 1;

                if (coeffAttr)
                {
                    if (!parseNumber(coeffAttr->Value(),&coeff))
                    {
                        throw EngineCreationException("Le coefficient d'une variable dans une contrainte de somme doit etre un entier.");
                    }
                }

                std::regex validPattern { "[[:alnum:]]+(\\[[[:digit:]]+([;][[:digit:]]+)?\\])*" };
                std::regex searchPatternVar { "([[:alnum:]]+)(\\[[[:digit:]]+([;][[:digit:]]+)?\\])*" };
                std::regex searchPatternIndexes { "[[:alnum:]]+((\\[[[:digit:]]+([;][[:digit:]]+)?\\])*)" };

                if(!std::regex_match(varStr, validPattern))
                {
                    err << "Mauvaise syntaxe de variable : " << varStr;
                    throw EngineCreationException(err.str());
                }

                std::string varName = regex_replace(varStr, searchPatternVar, "$1");
                std::string varIndexes = regex_replace(varStr, searchPatternIndexes, "$1");

                std::vector< std::vector<int> > varIndexesArray;

                loadIndexes(varIndexesArray,varIndexes);

                Indexer indexer;


                for (unsigned int i = 0; i < varIndexesArray.size(); i++)
                {
                    indexer.addIndex(varIndexesArray[i][1],varIndexesArray[i][0]);
                }

                if (indexer.numIndices() != 0)
                {
                    while (indexer.hasNext())
                    {
                        std::string dynVar = varName + indexer.next();

                        int indexVar = engine.getIndex(dynVar);

                        if (indexVar == -1)
                        {
                            err << "Cette variable n'existe pas : " << dynVar;
                            throw EngineCreationException(err.str());
                        }

                        VarCoeff temp;
                        temp.coeff = coeff;
                        temp.var = indexVar;
                        varsCoeff.push_back(temp);
                    }

                }

                else
                {
                    int indexVar = engine.getIndex(varStr);

                    if (indexVar == -1)
                    {
                        err << "Cette variable n'existe pas : " << varStr;
                        throw EngineCreationException(err.str());
                    }

                    VarCoeff temp;
                    temp.coeff = coeff;
                    temp.var = indexVar;
                    varsCoeff.push_back(temp);
                }


            }

            if (isValue)
            {
                constraints.addSumConstraint(varsCoeff,opStr,"number",value);
            }
            else
            {
                constraints.addSumConstraint(varsCoeff,opStr,"var",-1,variable);
            }
        }

    }
}


static void loadIndexes(std::vector< std::vector <int> >& indexes, std::string indexStr)
{

    if (indexStr.size() > 0)
    {
        std::regex variableIndexPattern { "([[:digit:]]+)([;]([[:digit:]]+))?" };

        std::smatch match;


        while (std::regex_search(indexStr, match, variableIndexPattern))
        {
            if (match.size() != 4)
            {
                throw EngineCreationException("Mauvaise syntaxe de variable (var1)");
            }

            std::vector<int> temp;
            int index1 = 0;
            int index2 = 0;
            if (match[0] != match[1])
            {
                std::string str(match[1]);
                std::string str2(match[3]);
                if (!parseNumber(str.c_str(),&index1))
                {
                    throw EngineCreationException("Mauvaise syntaxe de variable (var1)");
                }
                if (!parseNumber(str2.c_str(),&index2))
                {
                    throw EngineCreationException("Mauvaise syntaxe de variable (var1)");
                }

            }
            else
            {
                std::string str(match[0]);
                std::string str2(match[1]);
                if (!parseNumber(str.c_str(),&index1))
                {
                    throw EngineCreationException("Mauvaise syntaxe de variable (var1)");
                }
                if (!parseNumber(str2.c_str(),&index2))
                {
                    throw EngineCreationException("Mauvaise syntaxe de variable (var1)");
                }

            }

            temp.push_back(index1);
            temp.push_back(index2);

            indexes.push_back(temp);

            indexStr = match.suffix().str();
        }
    }


}

static void ReduceVariable(Variable* var, int value, std::string op)
{
    if (op == "=")
    {
        var->getDomain().removeLessThan(value);
        var->getDomain().removeGreaterThan(value);
    }
    else if (op == ">=")
        var->getDomain().removeLessThan(value);
    else if (op == ">")
        var->getDomain().removeLessThan(value, true);
    else if (op == "<=")
        var->getDomain().removeGreaterThan(value);
    else if (op == "<")
    {
        var->getDomain().removeGreaterThan(value, true);
    }

    else if (op == "!=")
        var->getDomain().remove(value);
}


static char GetOperator(std::string op)
{
    if (op == "=")
        return Constraints::BIN_CON_EQUALS;
    else if (op == ">=")
        return Constraints::BIN_CON_EQUALS | Constraints::BIN_CON_GREATER;
    else if (op == ">")
        return Constraints::BIN_CON_GREATER;
    else if (op == "<=")
        return Constraints::BIN_CON_EQUALS | Constraints::BIN_CON_LESS;
    else if (op == "<")
        return Constraints::BIN_CON_LESS;
    else if (op == "!=")
        return Constraints::BIN_CON_LESS | Constraints::BIN_CON_GREATER;
    else
        return -1;

}
