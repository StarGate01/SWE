#ifndef SWE_CDLDATA_HH
#define SWE_CDLDATA_HH

#include <string>
#include <map>
#include <vector>

using namespace std;

namespace parser
{

    enum class CDLPrimitiveTypes : unsigned int
    {
        Char,
        Byte,
        Short,
        Int,
        Float,
        Double,
        String
    };

    struct CDLDimension
    {

        string name;
        int length;
        bool unlimited;

    };

    struct CDLAttribute
    {

        string name;
        vector<string> values;

    };
    
    struct CDLVariable
    {

        string name;
        CDLPrimitiveTypes type;
        vector<string> components;
        map<string, CDLAttribute> attributes;
        vector<string> data;

    };

    struct CDLData
    {

        string name;
        map<string, CDLDimension> dimensions;
        map<string, CDLVariable> variables;

    };

}

#endif