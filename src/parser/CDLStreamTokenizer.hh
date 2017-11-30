#ifndef SWE_CDLSTREAMTOKENIZER_HH
#define SWE_CDLSTREAMTOKENIZER_HH

#include <string>
#include <queue>

using namespace std;

namespace parser
{

    struct Token
    {

        enum class Type : unsigned int
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

        Type type;
        string value;

        Token()
        {};

        Token(Type t, string v)
            : type(t),
              value(v)
        {};

    };

    class CDLStreamTokenizer
    {

        private:
            
            struct State
            {

                string literalBuffer = "";
                bool hadColon = false;
                char commentState = 0;
                bool verbatim = false;

            };

            State state;
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