#ifndef SWE_CDLSTREAMPARSER_HH
#define SWE_CDLSTREAMPARSER_HH

#include <queue>
#include "CDLData.hh"

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

    };

    struct Token
    {

        TokenType type;
        string value;

        Token()
        {};

        Token(TokenType t)
            : type(t)
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

    enum class StreamPosition : unsigned int
    {
        Start,
        Header,
        HeaderName,
        Outside,
        AwaitingDimSectorSeperator,
        AwaitingValSectorSeperator,
        AwaitingDatSectorSeperator,
        Dimensions,
        Variables,
        Data,
        End
    };

    enum class DimDatStreamPosition : unsigned int
    {
        Start,
        AwaitingAssignmentOperator,
        AwaitingValue
    };

    enum class VarStreamPosition : unsigned int
    {
        Start,
        AwaitingLeftParenthesis,
        AwaitingDimensionName,
        AwaitingMemberOperatorOrVariableName,
        AwaitingMemberName,
        AwaitingAssignmentOperator,
        AwaitingMemberValue
    };

    struct ParserProcessingState
    {

        StreamPosition position = StreamPosition::Start;
        DimDatStreamPosition dimPosition = DimDatStreamPosition::Start;
        VarStreamPosition varPosition = VarStreamPosition::Start;
        DimDatStreamPosition datPosition = DimDatStreamPosition::Start;

        vector<Token> currentLine;

        CDLDimension *currentDimension;
        CDLVariable *currentVariable;
        CDLAttribute *currentAttribute;

        Token lastLiteral;
        bool globalAttribute;

    };

    class CDLStreamParser
    {

        private:

            ParserProcessingState state;
            CDLData* data;
        
            void processDimensionalToken(Token t);
            void processVariableToken(Token t);
            void processDataToken(Token t);

        public:

            static CDLData* CDLStringToData(string s);

            CDLStreamParser(CDLData* d)
                : data(d)
            {};

            void processToken(Token t);

    };

}

#endif