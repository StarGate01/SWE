#include <stdexcept>

#include "CDLStreamTokenizer.hh"

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
        if(state.hadColon) tokens.push(Token(TokenType::SectorSeperator, ":"));
    }
    else if(state.hadColon) tokens.push(Token(TokenType::MemberOperator, ":"));
    state.hadColon = false;
    if(state.commentState == 2) return;
    if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '-' || c == '+')
        state.literalBuffer += c;
    else
    {
        if(state.verbatim)
        {
            if(c == '"') state.verbatim = false;
            else
            {
                state.literalBuffer += c;
                return;
            }
        }
        else if(c == '"') state.verbatim = true;
        if(state.literalBuffer.length() != 0)
        {
            tokens.push(Token(TokenType::Literal, state.literalBuffer));
            state.literalBuffer = "";
        }
        if(c == ';') tokens.push(Token(TokenType::Seperator, ";"));
        else if(c == ',') tokens.push(Token(TokenType::DataSeperator, ","));
        else if(c == '=') tokens.push(Token(TokenType::AssignmentOperator, "="));
        else if(c == ':') state.hadColon = true;
        else if(c == '(') tokens.push(Token(TokenType::LeftParenthesis, "("));
        else if(c == ')') tokens.push(Token(TokenType::RightParenthesis, ")"));
        else if(c == '{') tokens.push(Token(TokenType::LeftBrace, "{"));
        else if(c == '}') tokens.push(Token(TokenType::RightBrace, "}"));
        else if(c == '/' && state.commentState <= 2) state.commentState++;
    }

};
