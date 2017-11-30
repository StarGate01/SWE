#include <stdexcept>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "CDLStreamParser.hh"

using namespace parser;
using namespace std;

void CDLStreamParser::processToken(Token t)
{
    switch(state.position)
    {
        case StreamPosition::Start:
            if(t.type == TokenType::Literal && t.value == "netcdf") state.position = StreamPosition::Header;
            else throw runtime_error("Expected literal 'netcfd' for header");
            break;
        
        case StreamPosition::Header:
            if(t.type == TokenType::Literal)
            {
                data.name = t.value;
                state.position = StreamPosition::HeaderName;
            } 
            else throw runtime_error("Expected literal for header name");
            break;

        case StreamPosition::HeaderName:
            if(t.type == TokenType::LeftBrace) state.position = StreamPosition::Outside;
            else throw runtime_error("Expected '{' for header end");
            break;

        case StreamPosition::Outside:
            if(t.type == TokenType::Literal && t.value == "dimensions") 
                state.position = StreamPosition::AwaitingDimSectorSeperator;
            else throw runtime_error("Expected 'dimensions' for section name");
            break;

        case StreamPosition::AwaitingDimSectorSeperator:
            if(t.type == TokenType::SectorSeperator) state.position = StreamPosition::Dimensions;
            else throw runtime_error("Expected ':' for section dimensions beginning");
            break;

        case StreamPosition::Dimensions:
            if(t.type != TokenType::Seperator)
            {
                if(state.lineEmpty && t.type == TokenType::Literal && t.value == "variables")
                    state.position = StreamPosition::AwaitingValSectorSeperator;
                else
                {
                    state.lineEmpty = false;
                    processDimensionalToken(t);
                }
            } 
            else
            {
                state.subPosition = StreamPosition::Start;
                state.lineEmpty = true;
            }
            break;
        
        case StreamPosition::AwaitingValSectorSeperator:
            if(t.type == TokenType::SectorSeperator) state.position = StreamPosition::Variables;
            else state.position = StreamPosition::Dimensions;
            break;

        case StreamPosition::Variables:
            if(t.type != TokenType::Seperator)
            {
                if(state.lineEmpty && t.type == TokenType::Literal && t.value == "data")
                    state.position = StreamPosition::AwaitingDatSectorSeperator;
                else 
                {
                    state.lineEmpty = false;
                    processVariableToken(t);
                }
            } 
            else
            {
                state.subPosition = StreamPosition::Start;
                state.lineEmpty = true;
            }
            break;

        case StreamPosition::AwaitingDatSectorSeperator:
            if(t.type == TokenType::SectorSeperator) 
            {
                state.subPosition = StreamPosition::Start;
                state.position = StreamPosition::Data;
            }
            else state.position = StreamPosition::Variables;
            break;

        case StreamPosition::Data:
            if(t.type != TokenType::RightBrace) processDataToken(t);
            else state.position = StreamPosition::End;
            break;

        case StreamPosition::End:
            throw runtime_error("Expected no tokens after '}'");
            break;
        
        default: break;
    }
};

void CDLStreamParser::processDimensionalToken(Token t)
{
    switch(state.subPosition)
    {
        case StreamPosition::Start:
            if(t.type == TokenType::Literal)
            {
                data.dimensions[t.value] = CDLDimension({name: t.value});
                state.currentDimension = &(data.dimensions[t.value]);
                state.subPosition = StreamPosition::AwaitingAssignmentOperator;
            }
            else if (t.type != TokenType::DataSeperator) throw runtime_error("Expected literal for dimension name");
            break;

        case StreamPosition::AwaitingAssignmentOperator:
            if(t.type == TokenType::AssignmentOperator) state.subPosition = StreamPosition::AwaitingValue;
            else throw runtime_error("Expected '=' for dimension assignment");
            break;

        case StreamPosition::AwaitingValue:
            if(t.type == TokenType::Literal)
            {
                if(t.value == "unlimited" || t.value == "UNLIMITED") 
                    state.currentDimension->unlimited = true;
                else
                {
                    try { state.currentDimension->length = stoi(t.value); }
                    catch(...) { throw runtime_error("Expected dimension length to be integer"); }
                }
                state.subPosition = StreamPosition::Start;
            }
            else throw runtime_error("Expected literal for dimension length");
            break;
        
        default: break;
    }
};

void CDLStreamParser::processVariableToken(Token t)
{
    switch(state.subPosition)
    {
        case StreamPosition::Start:
            if(t.type == TokenType::Literal)
            {
                state.subPosition = StreamPosition::AwaitingMemberOperatorOrVariableName;
                state.lastLiteral = t;
            }
            else if(t.type == TokenType::MemberOperator) 
            {
                state.subPosition = StreamPosition::AwaitingMemberName;
                state.globalAttribute = true;
            }
            else throw runtime_error("Expected literal for variable type or variable, or ':' for global attribute");
            break;

        case StreamPosition::AwaitingMemberOperatorOrVariableName:
            if(t.type == TokenType::MemberOperator)
            {
                state.subPosition = StreamPosition::AwaitingMemberName;
                state.globalAttribute = false;
                map<string, ICDLVariable*>::iterator it = data.variables.find(state.lastLiteral.value);
                if(it != data.variables.end())
                {
                    state.currentVariable = it->second;
                    state.subPosition = StreamPosition::AwaitingMemberName;
                }
                else 
                {
                    stringstream ss;
                    ss << "Expected a previously declared variable, " << state.lastLiteral.value << " is undeclared";
                    throw runtime_error(ss.str()); 
                }
            }
            else if(t.type == TokenType::Literal)
            {
                string& ct = state.lastLiteral.value;
                transform(ct.begin(), ct.end(), ct.begin(), ::tolower);
                if(ct == "unsigned") 
                {
                    state.isUnsigned = true;
                    state.lastLiteral.value = t.value;
                }
                else
                {
                    #define CDL_CREATE(T) state.currentVariable = new CDLVariable<T>()
                    #define CDL_CREATE_SIGN(T) state.isUnsigned ? CDL_CREATE(u##T) : CDL_CREATE(T)
                    if(ct == "char") CDL_CREATE(int8_t);
                    else if(ct == "byte") CDL_CREATE_SIGN(int8_t);
                    else if(ct == "short") CDL_CREATE_SIGN(int16_t);
                    else if(ct == "int") CDL_CREATE_SIGN(int32_t);
                    else if(ct == "int64") CDL_CREATE_SIGN(int64_t);
                    else if(ct == "float") CDL_CREATE(float);
                    else if(ct == "double") CDL_CREATE(double);
                    else CDL_CREATE(string);
                    state.currentVariable->name = t.value;
                    state.currentVariable->type = state.lastLiteral.value;
                    state.currentVariable->isUnsigned = state.isUnsigned;
                    data.variables[t.value] = state.currentVariable;
                    state.subPosition = StreamPosition::AwaitingLeftParenthesis;
                    state.isUnsigned = false;
                }
            }
            else if(t.type != TokenType::DataSeperator) throw runtime_error("Expected literal, ':' or ','"); 
            break;

        case StreamPosition::AwaitingMemberName:
            if(t.type == TokenType::Literal)
            {
                if(!state.globalAttribute)
                {
                    state.currentVariable->attributes[t.value] = CDLAttribute({name: t.value});
                    state.currentAttribute = &(state.currentVariable->attributes[t.value]);
                }
                else
                {
                    data.globalAttributes[t.value] = CDLAttribute({name: t.value});
                    state.currentAttribute = &(data.globalAttributes[t.value]);
                }
                state.subPosition = StreamPosition::AwaitingAssignmentOperator;
            }
            else throw runtime_error("Expected literal for member name"); 
            break;

        case StreamPosition::AwaitingAssignmentOperator:
            if(t.type == TokenType::AssignmentOperator) state.subPosition = StreamPosition::AwaitingValue;
            else throw runtime_error("Expected '=' for attribute assignment");
            break;

        case StreamPosition::AwaitingValue:
            if(t.type == TokenType::Literal) state.currentAttribute->values.push_back(t.value);
            else if(t.type == TokenType::Seperator) state.subPosition = StreamPosition::Start;
            else if(t.type != TokenType::DataSeperator) throw runtime_error("Expected literal for attribute value");
            break;

        case StreamPosition::AwaitingLeftParenthesis:
            if(t.type == TokenType::LeftParenthesis) state.subPosition = StreamPosition::AwaitingDimensionName;
            else throw runtime_error("Expected '(' for variable declaration");
            break;
        
        case StreamPosition::AwaitingDimensionName:
            if(t.type == TokenType::Literal) 
            {
                map<string, CDLDimension>::iterator it = data.dimensions.find(t.value);
                if(it != data.dimensions.end()) state.currentVariable->components.push_back(t.value);
                else
                {
                    stringstream ss;
                    ss << "Expected a previously defined dimnsion, " << t.value << " is undefined";
                    throw runtime_error(ss.str()); 
                }
            }
            else if(t.type == TokenType::RightParenthesis) state.subPosition = StreamPosition::AwaitingMemberOperatorOrVariableName;
            else if(t.type != TokenType::DataSeperator) throw runtime_error("Expected literals for variable dimensions");
            break;

        default: break;
    }
};

void CDLStreamParser::processDataToken(Token t)
{
    switch(state.subPosition)
    {
        case StreamPosition::Start:
            if(t.type == TokenType::Literal)
            {
                map<string, ICDLVariable*>::iterator it = data.variables.find(t.value);
                if(it != data.variables.end())
                {
                    state.currentVariable = it->second;
                    state.subPosition = StreamPosition::AwaitingAssignmentOperator;
                }
                else
                {
                    stringstream ss;
                    ss << "Expected a previously declared variable, " << t.value << " is undeclared";
                    throw runtime_error(ss.str());
                }
            }
            else throw runtime_error("Expected literal for variable name");
            break;

        case StreamPosition::AwaitingAssignmentOperator:
            if(t.type == TokenType::AssignmentOperator) state.subPosition = StreamPosition::AwaitingValue;
            else throw runtime_error("Expected '=' for data assignment");
            break;

        case StreamPosition::AwaitingValue:
            if(t.type == TokenType::Literal) 
            {
                #define CDLCAST(T, A) (dynamic_cast<CDLVariable<T>*>(state.currentVariable))->data.push_back(A)
                #define CDLCAST_SIGN_EX(T, A1, A2) state.currentVariable->isUnsigned? CDLCAST(u##T, A1): CDLCAST(T, A2)
                #define CDLCAST_SIGN(T, A) CDLCAST_SIGN_EX(T, A, A)
                string& ct = state.currentVariable->type;
                try
                {
                    if(ct == "char") CDLCAST(int8_t, stoi(t.value));
                    else if(ct == "byte") CDLCAST_SIGN(int8_t, stoi(t.value));
                    else if(ct == "short") CDLCAST_SIGN(int16_t, stoi(t.value));
                    else if(ct == "int") CDLCAST_SIGN_EX(int32_t, stoul(t.value), stol(t.value));
                    else if(ct == "int64") CDLCAST_SIGN_EX(int64_t, stoull(t.value), stoll(t.value));
                    else if(ct == "float") CDLCAST(float, stof(t.value));
                    else if(ct == "double") CDLCAST(double, stod(t.value));
                    else CDLCAST(string, t.value);
                }
                catch(...) 
                { 
                    stringstream ss;
                    ss << "Expected data " << t.value << " to be of type " << (state.currentVariable->isUnsigned? "unsigned" : "") << ct;
                    throw runtime_error(ss.str()); 
                }
            }
            else if(t.type == TokenType::Seperator) state.subPosition = StreamPosition::Start;
            else if(t.type != TokenType::DataSeperator) throw runtime_error("Expected literal for data value");
            break;

        default: break;
    }
};

void CDLStreamParser::CDLStringToData(string s, CDLData& d)
{
    istringstream ss(s);
    return CDLStreamToData(ss, d);
};

void CDLStreamParser::CDLStreamToData(istream& s, CDLData& d)
{
    CDLStreamTokenizer tokenizer;
    CDLStreamParser parser(d);
    unsigned long int lineNumber = 1;
    unsigned int linePos = 1;
    char c;
    while(true)
    {
        c = s.get();
        if(c == EOF) break;
        tokenizer.read(c);
        linePos++;
        while(tokenizer.hasTokens()) 
        {
            try{ parser.processToken(tokenizer.getToken()); }
            catch(const runtime_error &e) 
            { 
                stringstream ss;
                ss << "Unexpected token at line " << lineNumber << ", column " << linePos << ": " << e.what();
                throw runtime_error(ss.str());
            }
        }
        if(c == '\n') 
        {
            lineNumber++;
            linePos = 1;
        }
    }
};