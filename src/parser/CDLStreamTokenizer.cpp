/**
 * @file CDLStreamTokenizer.cpp
 * @brief Implementation of CDLStreamTokenizer.hh
 */

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
        if(state.hadColon) tokens.push(Token(Token::Type::SectorSeperator, ":"));
    }
    else if(state.hadColon) tokens.push(Token(Token::Type::MemberOperator, ":"));
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
            tokens.push(Token(Token::Type::Literal, state.literalBuffer));
            state.literalBuffer = "";
        }
        if(c == ';') tokens.push(Token(Token::Type::Seperator, ";"));
        else if(c == ',') tokens.push(Token(Token::Type::DataSeperator, ","));
        else if(c == '=') tokens.push(Token(Token::Type::AssignmentOperator, "="));
        else if(c == ':') state.hadColon = true;
        else if(c == '(') tokens.push(Token(Token::Type::LeftParenthesis, "("));
        else if(c == ')') tokens.push(Token(Token::Type::RightParenthesis, ")"));
        else if(c == '{') tokens.push(Token(Token::Type::LeftBrace, "{"));
        else if(c == '}') tokens.push(Token(Token::Type::RightBrace, "}"));
        else if(c == '/' && state.commentState <= 2) state.commentState++;
    }

};
