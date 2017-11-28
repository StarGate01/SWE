#include <stdexcept>
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
                //TODO parse line in dimension sector
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
                //TODO parse line in variables sector
                state.currentLine.clear();
            }
            break;

        case StreamPosition::AwaitingDatSectorSeperator:
            if(t.type == TokenType::SectorSeperator) state.position = StreamPosition::Data;
            else state.position = StreamPosition::Variables;
            break;

        case StreamPosition::Data:
            if(state.activeDataAssignment)
            {
                if(t.type == TokenType::Literal) state.currentDataVariable->data.push_back(t.value);
                else if(t.type == TokenType::Seperator) state.activeDataAssignment = false;
                else if(t.type != TokenType::DataSeperator) throw runtime_error("Expected literal, ',' or ';' in data array");
            }
            else
            {
                if(t.type == TokenType::Literal)
                {
                    map<string, CDLVariable>::iterator it = data->variables.find(t.value);
                    if(it != data->variables.end())
                    {
                        state.currentDataVariable = &(it->second);
                        state.awaiting = TokenType::AssignmentOperator;
                    }
                    else throw runtime_error("Expected a previously declared variable"); 
                }
                else if(t.type == TokenType::AssignmentOperator && state.awaiting == TokenType::AssignmentOperator)
                {
                    state.activeDataAssignment = true;
                    state.awaiting = TokenType::None;
                }
                else if (t.type == TokenType::RightBrace && state.awaiting == TokenType::None) state.position = StreamPosition::End;
                else throw runtime_error("Expected literal, '}' or '=' after literal for data array");
            }
            break;

        case StreamPosition::End:
            throw runtime_error("Expected no tokens after '}'");
            break;

    }
};

CDLData* CDLStreamParser::CDLStringToData(string s)
{
    CDLData* ret = new CDLData();
    CDLStreamTokenizer tokenizer;
    CDLStreamParser parser(ret);
    for(char& c : s) 
    {
        tokenizer.read(c);
        // while(tokenizer.hasTokens()) parser.processToken(tokenizer.getToken());
    }
    while(tokenizer.hasTokens())
    {
        parser.processToken(tokenizer.getToken());
    }
    return ret;
};