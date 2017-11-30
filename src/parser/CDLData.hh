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
        };

    };

    struct CDLAttribute
    {

        string name;
        vector<string> values;

        bool operator== (const CDLAttribute &b) const
        {
            return (name == b.name && values == b.values);
        };

    };
    
    struct ICDLVariable 
    { 

        string type;
        bool isUnsigned;
        string name;
        vector<string> components;
        map<string, CDLAttribute> attributes;

        void assign(string t, bool u, string n, vector<string> c, map<string, CDLAttribute> a)
        {
            type = t;
            isUnsigned = u;
            name = n;
            components = c;
            attributes = a;
        };

        bool operator== (const ICDLVariable &b) const
        {
            return (name == b.name && type == b.type && components == b.components 
                && attributes == b.attributes);
        };

        virtual ~ICDLVariable()
        {};

    };

    template<typename T> struct CDLVariable : public ICDLVariable
    {

        vector<T> data;

        CDLVariable<T>()
        {};

        CDLVariable<T>(vector<T> d)
            : ICDLVariable(), 
              data(d)
        {};

        bool operator== (const CDLVariable<T> &b) const
        {
            return (ICDLVariable::operator==(b) && data == b.data);
        };

    };

    struct CDLData
    {

        string name;
        map<string, CDLAttribute> globalAttributes;
        map<string, CDLDimension> dimensions;
        map<string, ICDLVariable*> variables;

        bool operator== (const CDLData &b) const
        {
            if (!(name == b.name && globalAttributes == b.globalAttributes && dimensions == b.dimensions)) return false;
            if (variables.size() != b.variables.size()) return false;
            if (!(equal(begin(variables), end(variables), begin(b.variables),
                [] (const pair<string, ICDLVariable*> lhs, const pair<string, ICDLVariable*> rhs)
                { 
                    /// @cond CASTING_MACROS
                    #define CDL_EQ(T) (*(dynamic_cast<CDLVariable<T>*>(lhs.second))) == (*(dynamic_cast<CDLVariable<T>*>(rhs.second)))
                    #define CDL_NEQ_RET(T) { if(!(CDL_EQ(T))) return false; }
                    #define CDL_NEQ_RET_SIGN(T) { if(!((!lhs.second->isUnsigned && (CDL_EQ(T))) || (lhs.second->isUnsigned && (CDL_EQ(u##T))))) return false; }
                    /// @endcond
                    string& ct = lhs.second->type;
                    if(ct == "char") CDL_NEQ_RET(int8_t)
                    else if(ct == "byte") CDL_NEQ_RET_SIGN(int8_t)
                    else if(ct == "short") CDL_NEQ_RET_SIGN(int16_t)
                    else if(ct == "int") CDL_NEQ_RET_SIGN(int32_t)
                    else if(ct == "int64") CDL_NEQ_RET_SIGN(int64_t)
                    else if(ct == "float") CDL_NEQ_RET(float)
                    else if(ct == "double") CDL_NEQ_RET(double)
                    else CDL_NEQ_RET(string)
                    return true;
                }))) return false;
            return true;
        };

        ~CDLData()
        {
           for (auto & v : variables) delete v.second;
           variables.clear();
        }

    };

}

#endif