/**
 * @file CDLStreamParser.cpp
 * @brief Implementation of CDLStreamParser.hh
 */

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
        case State::StreamPosition::Start:
            if(t.type == Token::Type::Literal && t.value == "netcdf") state.position = State::StreamPosition::Header;
            else throw runtime_error("Expected literal 'netcfd' for header");
            break;
        
        case State::StreamPosition::Header:
            if(t.type == Token::Type::Literal)
            {
                data.name = t.value;
                state.position = State::StreamPosition::HeaderName;
            } 
            else throw runtime_error("Expected literal for header name");
            break;

        case State::StreamPosition::HeaderName:
            if(t.type == Token::Type::LeftBrace) state.position = State::StreamPosition::Outside;
            else throw runtime_error("Expected '{' for header end");
            break;

        case State::StreamPosition::Outside:
            if(t.type == Token::Type::Literal && t.value == "dimensions") 
                state.position = State::StreamPosition::AwaitingDimSectorSeperator;
            else throw runtime_error("Expected 'dimensions' for section name");
            break;

        case State::StreamPosition::AwaitingDimSectorSeperator:
            if(t.type == Token::Type::SectorSeperator) state.position = State::StreamPosition::Dimensions;
            else throw runtime_error("Expected ':' for section dimensions beginning");
            break;

        case State::StreamPosition::Dimensions:
            if(t.type != Token::Type::Seperator)
            {
                if(state.lineEmpty && t.type == Token::Type::Literal && t.value == "variables")
                    state.position = State::StreamPosition::AwaitingValSectorSeperator;
                else
                {
                    state.lineEmpty = false;
                    processDimensionalToken(t);
                }
            } 
            else
            {
                state.subPosition = State::StreamPosition::Start;
                state.lineEmpty = true;
            }
            break;
        
        case State::StreamPosition::AwaitingValSectorSeperator:
            if(t.type == Token::Type::SectorSeperator) state.position = State::StreamPosition::Variables;
            else state.position = State::StreamPosition::Dimensions;
            break;

        case State::StreamPosition::Variables:
            if(t.type != Token::Type::Seperator)
            {
                if(state.lineEmpty && t.type == Token::Type::Literal && t.value == "data")
                    state.position = State::StreamPosition::AwaitingDatSectorSeperator;
                else 
                {
                    state.lineEmpty = false;
                    processVariableToken(t);
                }
            } 
            else
            {
                state.subPosition = State::StreamPosition::Start;
                state.lineEmpty = true;
            }
            break;

        case State::StreamPosition::AwaitingDatSectorSeperator:
            if(t.type == Token::Type::SectorSeperator) 
            {
                state.subPosition = State::StreamPosition::Start;
                state.position = State::StreamPosition::Data;
            }
            else state.position = State::StreamPosition::Variables;
            break;

        case State::StreamPosition::Data:
            if(t.type != Token::Type::RightBrace) processDataToken(t);
            else state.position = State::StreamPosition::End;
            break;

        case State::StreamPosition::End:
            throw runtime_error("Expected no tokens after '}'");
            break;
        
        default: break;
    }
};

void CDLStreamParser::processDimensionalToken(Token t)
{
    switch(state.subPosition)
    {
        case State::StreamPosition::Start:
            if(t.type == Token::Type::Literal)
            {
                data.dimensions[t.value] = CDLDimension({name: t.value});
                state.currentDimension = &(data.dimensions[t.value]);
                state.subPosition = State::StreamPosition::AwaitingAssignmentOperator;
            }
            else if (t.type != Token::Type::DataSeperator) throw runtime_error("Expected literal for dimension name");
            break;

        case State::StreamPosition::AwaitingAssignmentOperator:
            if(t.type == Token::Type::AssignmentOperator) state.subPosition = State::StreamPosition::AwaitingValue;
            else throw runtime_error("Expected '=' for dimension assignment");
            break;

        case State::StreamPosition::AwaitingValue:
            if(t.type == Token::Type::Literal)
            {
                if(t.value == "unlimited" || t.value == "UNLIMITED") 
                    state.currentDimension->unlimited = true;
                else
                {
                    try { state.currentDimension->length = stoi(t.value); }
                    catch(...) { throw runtime_error("Expected dimension length to be integer"); }
                }
                state.subPosition = State::StreamPosition::Start;
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
        case State::StreamPosition::Start:
            if(t.type == Token::Type::Literal)
            {
                state.subPosition = State::StreamPosition::AwaitingMemberOperatorOrVariableName;
                state.lastLiteral = t;
            }
            else if(t.type == Token::Type::MemberOperator) 
            {
                state.subPosition = State::StreamPosition::AwaitingMemberName;
                state.globalAttribute = true;
            }
            else throw runtime_error("Expected literal for variable type or variable, or ':' for global attribute");
            break;

        case State::StreamPosition::AwaitingMemberOperatorOrVariableName:
            if(t.type == Token::Type::MemberOperator)
            {
                state.subPosition = State::StreamPosition::AwaitingMemberName;
                state.globalAttribute = false;
                map<string, ICDLVariable*>::iterator it = data.variables.find(state.lastLiteral.value);
                if(it != data.variables.end())
                {
                    state.currentVariable = it->second;
                    state.subPosition = State::StreamPosition::AwaitingMemberName;
                }
                else 
                {
                    stringstream ss;
                    ss << "Expected a previously declared variable, " << state.lastLiteral.value << " is undeclared";
                    throw runtime_error(ss.str()); 
                }
            }
            else if(t.type == Token::Type::Literal)
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
                    state.subPosition = State::StreamPosition::AwaitingLeftParenthesis;
                    state.isUnsigned = false;
                }
            }
            else if(t.type != Token::Type::DataSeperator) throw runtime_error("Expected literal, ':' or ','"); 
            break;

        case State::StreamPosition::AwaitingMemberName:
            if(t.type == Token::Type::Literal)
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
                state.subPosition = State::StreamPosition::AwaitingAssignmentOperator;
            }
            else throw runtime_error("Expected literal for member name"); 
            break;

        case State::StreamPosition::AwaitingAssignmentOperator:
            if(t.type == Token::Type::AssignmentOperator) state.subPosition = State::StreamPosition::AwaitingValue;
            else throw runtime_error("Expected '=' for attribute assignment");
            break;

        case State::StreamPosition::AwaitingValue:
            if(t.type == Token::Type::Literal) state.currentAttribute->values.push_back(t.value);
            else if(t.type == Token::Type::Seperator) state.subPosition = State::StreamPosition::Start;
            else if(t.type != Token::Type::DataSeperator) throw runtime_error("Expected literal for attribute value");
            break;

        case State::StreamPosition::AwaitingLeftParenthesis:
            if(t.type == Token::Type::LeftParenthesis) state.subPosition = State::StreamPosition::AwaitingDimensionName;
            else throw runtime_error("Expected '(' for variable declaration");
            break;
        
        case State::StreamPosition::AwaitingDimensionName:
            if(t.type == Token::Type::Literal) 
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
            else if(t.type == Token::Type::RightParenthesis) state.subPosition = State::StreamPosition::AwaitingMemberOperatorOrVariableName;
            else if(t.type != Token::Type::DataSeperator) throw runtime_error("Expected literals for variable dimensions");
            break;

        default: break;
    }
};

void CDLStreamParser::processDataToken(Token t)
{
    switch(state.subPosition)
    {
        case State::StreamPosition::Start:
            if(t.type == Token::Type::Literal)
            {
                map<string, ICDLVariable*>::iterator it = data.variables.find(t.value);
                if(it != data.variables.end())
                {
                    state.currentVariable = it->second;
                    state.subPosition = State::StreamPosition::AwaitingAssignmentOperator;
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

        case State::StreamPosition::AwaitingAssignmentOperator:
            if(t.type == Token::Type::AssignmentOperator) state.subPosition = State::StreamPosition::AwaitingValue;
            else throw runtime_error("Expected '=' for data assignment");
            break;

        case State::StreamPosition::AwaitingValue:
            if(t.type == Token::Type::Literal) 
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
            else if(t.type == Token::Type::Seperator) state.subPosition = State::StreamPosition::Start;
            else if(t.type != Token::Type::DataSeperator) throw runtime_error("Expected literal for data value");
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