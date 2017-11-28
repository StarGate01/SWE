#ifndef SWE_CDLDATA_HH
#define SWE_CDLDATA_HH

#include <string>
#include <map>
#include <vector>

using namespace std;

namespace parser
{

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
        string type;
        vector<string> components;
        map<string, CDLAttribute> attributes;
        vector<string> data;

    };

    struct CDLData
    {

        string name;
        map<string, CDLAttribute> globalAttributes;
        map<string, CDLDimension> dimensions;
        map<string, CDLVariable> variables;

    };

}

#endif