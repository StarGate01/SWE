/**
 * @file CDLData.hh
 * @brief Data structures which represent a CDL file
 */

#ifndef SWE_CDLDATA_HH
#define SWE_CDLDATA_HH

#include <string>
#include <map>
#include <vector>

using namespace std;

namespace parser
{

    /**
     * @brief Represents a NetCDF dimension
     */
    struct CDLDimension
    {

        string name; /**< The name */
        int length; /**< The length of the corresponding data array */
        bool unlimited; /**< Whether the length of the corresponding data array is fixed or not */

        bool operator== (const CDLDimension &b) const
        {
            return (length == b.length && unlimited == b.unlimited && name == b.name);
        };

    };

    /**
     * @brief Represents an attribute of a NetCDF variable
     */
    struct CDLAttribute
    {

        string name; /**< The name */
        vector<string> values; /**< The values, left to right */

        bool operator== (const CDLAttribute &b) const
        {
            return (name == b.name && values == b.values);
        };

    };
    
    /**
     * @brief Represents a generic (not typed) NetCDF variable
     */
    struct ICDLVariable 
    { 

        string type; /**< The data type */
        bool isUnsigned; /**< Whether the type is unsigned or signed */
        string name; /**< The name */
        vector<string> components; /**< The dimensional components */
        map<string, CDLAttribute> attributes; /**< The attributes attached to this variable */

        /**
         * @brief Re-assigns all member values
         * 
         * @param type ICDLVariable::type
         * @param isUnsigned ICDLVariable::isUnsigned
         * @param name ICDLVariable::name
         * @param components ICDLVariable::components
         * @param attributes ICDLVariable::attributes
         * 
         * This method is uses as a deferred constructor
         */
        void assign(string type, bool isUnsigned, string name, vector<string> components, map<string, CDLAttribute> attributes)
        {
            this->type = type;
            this->isUnsigned = isUnsigned;
            this->name = name;
            this->components = components;
            this->attributes = attributes;
        };

        bool operator== (const ICDLVariable &b) const
        {
            return (name == b.name && type == b.type && components == b.components 
                && attributes == b.attributes);
        };

        virtual ~ICDLVariable()
        {};

    };

    /**
     * @brief Represents a strongly typed variable
     */
    template<typename T> struct CDLVariable : public ICDLVariable
    {

        vector<T> data; /**< The actual data array */

        CDLVariable<T>()
        {};

        /**
         * @brief Constructs a variable with initial data
         * 
         * @param data the initial data
         */
        CDLVariable<T>(vector<T> data)
            : ICDLVariable(), 
              data(data)
        {};

        bool operator== (const CDLVariable<T> &b) const
        {
            return (ICDLVariable::operator==(b) && data == b.data);
        };

    };

    /**
     * @brief Represents the data inside a NetCDF file
     */
    struct CDLData
    {

        string name; /**< The name **/
        map<string, CDLAttribute> globalAttributes; /**< The global attributes */
        map<string, CDLDimension> dimensions; /**< The available dimensions */

        /**
         * @brief The defined variables
         * 
         * This map only stores pointers to generic variables, which have to be cast to a fitting type
         *  (depending on ICDLVariable::type) using std::dynamic_cast<CDLVariable<T>*>
         */
        map<string, ICDLVariable*> variables;

        bool operator== (const CDLData &b) const
        {
            if (!(name == b.name && globalAttributes == b.globalAttributes && dimensions == b.dimensions)) return false;
            if (variables.size() != b.variables.size()) return false;
            if (!(equal(begin(variables), end(variables), begin(b.variables),
                [] (const pair<string, ICDLVariable*> lhs, const pair<string, ICDLVariable*> rhs)
                { 
                    #define CDL_EQ(T) (*(dynamic_cast<CDLVariable<T>*>(lhs.second))) == (*(dynamic_cast<CDLVariable<T>*>(rhs.second)))
                    #define CDL_NEQ_RET(T) { if(!(CDL_EQ(T))) return false; }
                    #define CDL_NEQ_RET_SIGN(T) { if(!((!lhs.second->isUnsigned && (CDL_EQ(T))) || (lhs.second->isUnsigned && (CDL_EQ(u##T))))) return false; }
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