#ifndef SWE_CDLSTREAMTOKENIZER_HH
#define SWE_CDLSTREAMTOKENIZER_HH

#include <string>
#include <queue>

using namespace std;

namespace parser
{

    enum class TokenType : unsigned int
    { 
        None,
        Literal,
        Seperator,
        DataSeperator,
        SectorSeperator,
        AssignmentOperator,
        MemberOperator,
        LeftParenthesis,
        RightParenthesis,
        LeftBrace,
        RightBrace
    };

    struct TokenProcessingState
    {

        string literalBuffer = "";
        bool hadColon = false;
        char commentState = 0;
        bool verbatim = false;

    };

    struct Token
    {

        TokenType type;
        string value;

        Token()
        {};

        Token(TokenType t, string v)
            : type(t),
              value(v)
        {};

    };

    class CDLStreamTokenizer
    {

        private:
            
            TokenProcessingState state;
            queue<Token> tokens;

        public:

            CDLStreamTokenizer() 
            {};

            bool hasTokens();
            Token getToken();
            void read(char c);

    };

}

#endif