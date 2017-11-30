#ifndef SWE_CDLSTREAMPARSER_HH
#define SWE_CDLSTREAMPARSER_HH

#include <queue>
#include "CDLStreamTokenizer.hh"
#include "CDLData.hh"

using namespace std;

namespace parser
{

    enum class StreamPosition : unsigned int
    {
        Start,
        Header,
        HeaderName,
        Outside,
        AwaitingDimSectorSeperator,
        AwaitingValSectorSeperator,
        AwaitingDatSectorSeperator,
        AwaitingAssignmentOperator,
        AwaitingValue,
        AwaitingLeftParenthesis,
        AwaitingDimensionName,
        AwaitingMemberOperatorOrVariableName,
        AwaitingMemberName,
        Dimensions,
        Variables,
        Data,
        End
    };

    struct ParserProcessingState
    {

        StreamPosition position = StreamPosition::Start;
        StreamPosition subPosition = StreamPosition::Start;

        CDLDimension* currentDimension;
        ICDLVariable* currentVariable;
        CDLAttribute* currentAttribute;

        Token lastLiteral;
        bool globalAttribute = false;
        bool lineEmpty = false;
        bool isUnsigned = false;

    };

    class CDLStreamParser
    {

        private:

            ParserProcessingState state;
            CDLData& data;

            void processDimensionalToken(Token t);
            void processVariableToken(Token t);
            void processDataToken(Token t);

        public:

            static void CDLStringToData(string s, CDLData& d);
            static void CDLStreamToData(istream& s, CDLData& d);

            CDLStreamParser(CDLData& d)
                : data(d)
            {};

            void processToken(Token t);
            CDLData retrieve() { return data; }

    };

}

#endif