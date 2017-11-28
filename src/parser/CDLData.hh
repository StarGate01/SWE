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

        bool operator== (const CDLDimension &b) const
        {
            return (length == b.length && unlimited == b.unlimited && name == b.name);
        }

    };

    struct CDLAttribute
    {

        string name;
        vector<string> values;

        bool operator== (const CDLAttribute &b) const
        {
            return (name == b.name && values == b.values);
        }

    };
    
    struct CDLVariable
    {

        string name;
        string type;
        vector<string> components;
        map<string, CDLAttribute> attributes;
        vector<string> data;

        bool operator== (const CDLVariable &b) const
        {
            return (name == b.name && type == b.type 
                && components == b.components && attributes == b.attributes && data == b.data);
        }

    };

    struct CDLData
    {

        string name;
        map<string, CDLAttribute> globalAttributes;
        map<string, CDLDimension> dimensions;
        map<string, CDLVariable> variables;

        bool operator== (const CDLData &b) const
        {
            return (name == b.name && globalAttributes == b.globalAttributes
                && dimensions == b.dimensions && variables == b.variables);
        }

    };

}

#endif