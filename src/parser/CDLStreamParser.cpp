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
            {
                state.position = StreamPosition::AwaitingDimSectorSeperator;
            } 
            else throw runtime_error("Expected 'dimensions' for section name");
            break;

        case StreamPosition::AwaitingDimSectorSeperator:
            if(t.type == TokenType::SectorSeperator) state.position = StreamPosition::Dimensions;
            else throw runtime_error("Expected ':' for section dimensions beginning");
            break;

        case StreamPosition::Dimensions:
            if(t.type != TokenType::Seperator)
            {
                if(state.currentLine.size() == 0 
                    && t.type == TokenType::Literal && t.value.compare("variables") == 0)
                {
                    state.position = StreamPosition::AwaitingValSectorSeperator;
                }
                else state.currentLine.push_back(t);
            } 
            else
            {
                state.dimPosition = DimDatStreamPosition::Start;
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
                if(state.currentLine.size() == 0 
                    && t.type == TokenType::Literal && t.value.compare("data") == 0)
                {
                    state.position = StreamPosition::AwaitingDatSectorSeperator;
                } else state.currentLine.push_back(t);
            } 
            else
            {
                state.varPosition = VarStreamPosition::Start;
                for(Token const& tn: state.currentLine) processVariableToken(tn);
                state.currentLine.clear();
            }
            break;

        case StreamPosition::AwaitingDatSectorSeperator:
            if(t.type == TokenType::SectorSeperator) state.position = StreamPosition::Data;
            else state.position = StreamPosition::Variables;
            break;

        case StreamPosition::Data:
            processDataToken(t);
            break;

            // if(state.activeDataAssignment)
            // {
            //     if(t.type == TokenType::Literal) state.currentVariable->data.push_back(t.value);
            //     else if(t.type == TokenType::Seperator) state.activeDataAssignment = false;
            //     else if(t.type != TokenType::DataSeperator) throw runtime_error("Expected literal, ',' or ';' in data array");
            // }
            // else
            // {
            //     if(t.type == TokenType::Literal)
            //     {
            //         map<string, CDLVariable>::iterator it = data->variables.find(t.value);
            //         if(it != data->variables.end())
            //         {
            //             state.currentVariable = &(it->second);
            //             state.awaiting = TokenType::AssignmentOperator;
            //         }
            //         else throw runtime_error("Expected a previously declared variable"); 
            //     }
            //     else if(t.type == TokenType::AssignmentOperator && state.awaiting == TokenType::AssignmentOperator)
            //     {
            //         state.activeDataAssignment = true;
            //         state.awaiting = TokenType::None;
            //     }
            //     else if (t.type == TokenType::RightBrace && state.awaiting == TokenType::None) state.position = StreamPosition::End;
            //     else throw runtime_error("Expected literal, '}' or '=' after literal for data array");
            // }
            // break;

        case StreamPosition::End:
            throw runtime_error("Expected no tokens after '}'");
            break;
    }
};

void CDLStreamParser::processDimensionalToken(Token t)
{
    switch(state.dimPosition)
    {
        case DimDatStreamPosition::Start:
            if(t.type == TokenType::Literal)
            {
                data->dimensions[t.value] = CDLDimension({name: t.value});
                state.currentDimension = &(data->dimensions[t.value]);
                state.dimPosition = DimDatStreamPosition::AwaitingAssignmentOperator;
            }
            else if (t.type != TokenType::DataSeperator) throw runtime_error("Expected literal for dimension name");
            break;

        case DimDatStreamPosition::AwaitingAssignmentOperator:
            if(t.type == TokenType::AssignmentOperator) state.dimPosition = DimDatStreamPosition::AwaitingValue;
            else throw runtime_error("Expected '=' for dimension assignment");
            break;

        case DimDatStreamPosition::AwaitingValue:
            if(t.type == TokenType::Literal)
            {
                if(t.value.compare("unlimited") == 0) state.currentDimension->unlimited = true;
                else
                {
                    try { state.currentDimension->length = stoi(t.value); }
                    catch(...) { throw runtime_error("Expected dimension length to be integer"); }
                }
                state.dimPosition = DimDatStreamPosition::Start;
            }
            else throw runtime_error("Expected literal for dimension length");
            break;
    }
};

void CDLStreamParser::processVariableToken(Token t)
{
    switch(state.varPosition)
    {
        case VarStreamPosition::Start:
            if(t.type == TokenType::Literal)
            {
                state.varPosition = VarStreamPosition::AwaitingMemberOperatorOrVariableName;
                state.lastLiteral = t;
            }
            else if(t.type == TokenType::MemberOperator) 
            {
                state.varPosition = VarStreamPosition::AwaitingMemberName;
                state.globalAttribute = true;
            }
            else throw runtime_error("Expected literal for variable type or variable, or ':' for global attribute");
            break;

        case VarStreamPosition::AwaitingMemberOperatorOrVariableName:
            if(t.type == TokenType::MemberOperator)
            {
                state.varPosition = VarStreamPosition::AwaitingMemberName;
                state.globalAttribute = false;
                map<string, CDLVariable>::iterator it = data->variables.find(state.lastLiteral.value);
                if(it != data->variables.end())
                {
                    state.currentVariable = &(it->second);
                    state.varPosition = VarStreamPosition::AwaitingMemberName;
                }
                else throw runtime_error("Expected a previously declared variable"); 
            }
            else if(t.type == TokenType::Literal)
            {
                data->variables[t.value] = CDLVariable({name: t.value, type: state.lastLiteral.value});
                state.currentVariable = &(data->variables[t.value]);
                state.varPosition = VarStreamPosition::AwaitingLeftParenthesis;
            }
            else if(t.type != TokenType::DataSeperator) throw runtime_error("Expected literal, ':' or ','"); 
            break;

        case VarStreamPosition::AwaitingMemberName:
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
                state.varPosition = VarStreamPosition::AwaitingAssignmentOperator;
            }
            else throw runtime_error("Expected literal for member name"); 
            break;

        case VarStreamPosition::AwaitingAssignmentOperator:
            if(t.type == TokenType::AssignmentOperator) state.varPosition = VarStreamPosition::AwaitingMemberValue;
            else throw runtime_error("Expected '=' for attribute assignment");
            break;

        case VarStreamPosition::AwaitingMemberValue:
            if(t.type == TokenType::Literal) state.currentAttribute->values.push_back(t.value);
            else if(t.type == TokenType::Seperator) state.varPosition = VarStreamPosition::Start;
            else if(t.type != TokenType::DataSeperator) throw runtime_error("Expected literal for attribute value");
            break;

        case VarStreamPosition::AwaitingLeftParenthesis:
            if(t.type == TokenType::LeftParenthesis) state.varPosition = VarStreamPosition::AwaitingDimensionName;
            else throw runtime_error("Expected '(' for variable declaration");
            break;
        
        case VarStreamPosition::AwaitingDimensionName:
            if(t.type == TokenType::Literal) state.currentVariable->components.push_back(t.value);
            else if(t.type == TokenType::RightParenthesis) state.varPosition = VarStreamPosition::AwaitingMemberOperatorOrVariableName;
            else if(t.type != TokenType::DataSeperator) throw runtime_error("Expected literals for variable dimensions");
            break;

    }
};

void CDLStreamParser::processDataToken(Token t)
{
    switch(state.datPosition)
    {
        case DimDatStreamPosition::Start:
            if(t.type == TokenType::Literal)
            {
                map<string, CDLVariable>::iterator it = data->variables.find(t.value);
                if(it != data->variables.end())
                {
                    state.currentVariable = &(it->second);
                    state.datPosition = DimDatStreamPosition::AwaitingAssignmentOperator;
                }
                else throw runtime_error("Expected a previously declared variable"); 
            }
            else throw runtime_error("Expected literal for variable name");
            break;

        case DimDatStreamPosition::AwaitingAssignmentOperator:
            if(t.type == TokenType::AssignmentOperator) state.datPosition = DimDatStreamPosition::AwaitingValue;
            else throw runtime_error("Expected '=' for data assignment");
            break;

        case DimDatStreamPosition::AwaitingValue:
            if(t.type == TokenType::Literal) state.currentVariable->data.push_back(t.value);
            else if(t.type == TokenType::Seperator) sate.datPosition = DimDatStreamPosition::Start;
            else if(t.type != TokenType::DataSeperator) throw runtime_error("Expected literal for data value");
            break;
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