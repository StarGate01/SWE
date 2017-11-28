#include <stdexcept>
#include <sstream>
#include <iostream>

#include "CDLStreamParser.hh"

using namespace parser;
using namespace std;

bool CDLStreamTokenizer::hasTokens()
{
    return !tokens.empty();
}

Token CDLStreamTokenizer::getToken()
{
    if(tokens.empty()) throw runtime_error("Token queue is empty");
    Token result = tokens.front();
    tokens.pop();
    return result;
};

void CDLStreamTokenizer::read(char c)
{
    if(c == '\n')
    {
        state.commentState = 0;
        if(state.hadColon) tokens.push(Token(TokenType::SectorSeperator));
    }
    else if(state.hadColon) tokens.push(Token(TokenType::MemberOperator));
    state.hadColon = false;

    if(state.commentState == 2) return;
    if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '_')
    {
        state.literalBuffer += c;
    }
    else
    {
        if(state.literalBuffer.length() != 0)
        {
            tokens.push(Token(TokenType::Literal, state.literalBuffer));
            state.literalBuffer = "";
        }
        if(c == ';') tokens.push(Token(TokenType::Seperator));
        else if(c == ',') tokens.push(Token(TokenType::DataSeperator));
        else if(c == '=') tokens.push(Token(TokenType::AssignmentOperator));
        else if(c == ':') state.hadColon = true;
        else if(c == '(') tokens.push(Token(TokenType::LeftParenthesis));
        else if(c == ')') tokens.push(Token(TokenType::RightParenthesis));
        else if(c == '{') tokens.push(Token(TokenType::LeftBrace));
        else if(c == '}') tokens.push(Token(TokenType::RightBrace));
        else if(c == '/' && state.commentState <= 2) state.commentState++;
    }
};

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
                data->name = t.value;
                state.position = StreamPosition::HeaderName;
            } 
            else throw runtime_error("Expected literal for header name");
            break;

        case StreamPosition::HeaderName:
            if(t.type == TokenType::LeftBrace) state.position = StreamPosition::Outside;
            else throw runtime_error("Expected '{' for header end");
            break;

        case StreamPosition::Outside:
            if(t.type == TokenType::Literal && t.value.compare("dimensions") == 0) 
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
                if(state.currentLine.size() == 0 && t.type == TokenType::Literal && t.value.compare("variables") == 0)
                    state.position = StreamPosition::AwaitingValSectorSeperator;
                else state.currentLine.push_back(t);
            } 
            else
            {
                state.subPosition = StreamPosition::Start;
                for(Token const& tn: state.currentLine) processDimensionalToken(tn);
                state.currentLine.clear();
            }
            break;
        
        case StreamPosition::AwaitingValSectorSeperator:
            if(t.type == TokenType::SectorSeperator) state.position = StreamPosition::Variables;
            else state.position = StreamPosition::Dimensions;
            break;

        case StreamPosition::Variables:
            if(t.type != TokenType::Seperator)
            {
                if(state.currentLine.size() == 0  && t.type == TokenType::Literal && t.value.compare("data") == 0)
                    state.position = StreamPosition::AwaitingDatSectorSeperator;
                else state.currentLine.push_back(t);
            } 
            else
            {
                state.subPosition = StreamPosition::Start;
                for(Token const& tn: state.currentLine) processVariableToken(tn);
                state.currentLine.clear();
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
                data->dimensions[t.value] = CDLDimension({name: t.value});
                state.currentDimension = &(data->dimensions[t.value]);
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
                if(t.value.compare("unlimited") == 0 || t.value.compare("UNLIMITED") == 0) 
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
                map<string, CDLVariable>::iterator it = data->variables.find(state.lastLiteral.value);
                if(it != data->variables.end())
                {
                    state.currentVariable = &(it->second);
                    state.subPosition = StreamPosition::AwaitingMemberName;
                }
                else throw runtime_error("Expected a previously declared variable"); 
            }
            else if(t.type == TokenType::Literal)
            {
                data->variables[t.value] = CDLVariable({name: t.value, type: state.lastLiteral.value});
                state.currentVariable = &(data->variables[t.value]);
                state.subPosition = StreamPosition::AwaitingLeftParenthesis;
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
                    data->globalAttributes[t.value] = CDLAttribute({name: t.value});
                    state.currentAttribute = &(data->globalAttributes[t.value]);
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
            if(t.type == TokenType::Literal) state.currentVariable->components.push_back(t.value);
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
                map<string, CDLVariable>::iterator it = data->variables.find(t.value);
                if(it != data->variables.end())
                {
                    state.currentVariable = &(it->second);
                    state.subPosition = StreamPosition::AwaitingAssignmentOperator;
                }
                else throw runtime_error("Expected a previously declared variable"); 
            }
            else throw runtime_error("Expected literal for variable name");
            break;

        case StreamPosition::AwaitingAssignmentOperator:
            if(t.type == TokenType::AssignmentOperator) state.subPosition = StreamPosition::AwaitingValue;
            else throw runtime_error("Expected '=' for data assignment");
            break;

        case StreamPosition::AwaitingValue:
            if(t.type == TokenType::Literal) state.currentVariable->data.push_back(t.value);
            else if(t.type == TokenType::Seperator) state.subPosition = StreamPosition::Start;
            else if(t.type != TokenType::DataSeperator) throw runtime_error("Expected literal for data value");
            break;

        default: break;
    }
};


CDLData* CDLStreamParser::CDLStringToData(string s)
{
    CDLData* ret = new CDLData();
    CDLStreamTokenizer tokenizer;
    CDLStreamParser parser(ret);
    unsigned long int lineNumber = 0;
    unsigned int linePos = 0;
    for(char& c : s) 
    {
        tokenizer.read(c);
        linePos++;
        while(tokenizer.hasTokens()) 
        {
            try{ parser.processToken(tokenizer.getToken()); }
            catch(const runtime_error &e) 
            { 
                stringstream ss;
                ss << "At line " << lineNumber << ", column " << linePos << ": " << e.what();
                string error = ss.str();
                cout << error;
            }
        }
        if(c == '\n') 
        {
            lineNumber++;
            linePos = 0;
        }
    }
    return ret;
};