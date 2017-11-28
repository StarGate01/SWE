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

        bool operator== (const CDLDimension &b)
        {
            return (length == b.length && unlimited == b.unlimited && name.compare(b.name) == 0);
        }

    };

    struct CDLAttribute
    {

        string name;
        vector<string> values;

        bool operator== (const CDLAttribute &b)
        {
            return (name.compare(b.name) == 0 && values == b.values);
        }

    };
    
    struct CDLVariable
    {

        string name;
        string type;
        vector<string> components;
        map<string, CDLAttribute> attributes;
        vector<string> data;

        bool operator== (const CDLVariable &b)
        {
            return (name.compare(b.name) == 0 && type.compare(b.type) == 0 
                && components == b.components && attributes == b.attributes && data == b.data);
        }

    };

    struct CDLData
    {

        string name;
        map<string, CDLAttribute> globalAttributes;
        map<string, CDLDimension> dimensions;
        map<string, CDLVariable> variables;

        bool operator== (const CDLData &b)
        {
            return (name.compare(b.name) == 0 && globalAttributes == b.globalAttributes 
                && dimensions == b.dimensions && variables == b.variables);
        }

    };

}

#endif