/**
 * @file CDLStreamParser.hh
 * @brief Defines the CDL stream parser
 */

#ifndef SWE_CDLSTREAMPARSER_HH
#define SWE_CDLSTREAMPARSER_HH

#include <queue>
#include "CDLStreamTokenizer.hh"
#include "CDLData.hh"

using namespace std;

/**
 * @brief Contains classes related to parsing CDL files
 */
namespace parser
{

    /**
     * @brief Provides methods to parse a CDL file into a Data object
     */
    class CDLStreamParser
    {

        private:

            /**
             * @brief The internal state of CDLStreamParser
             */
            struct State
            {

                /**
                 * @brief The possible states of the section and subsection parser state machines
                 */
                enum class StreamPosition : unsigned int
                {
                    Start, /**< The idle state */
                    Header, /**< At the start of the file */
                    HeaderName, /**< After reading the file name */
                    Outside, /**< Inside the file, but outside any subsections */
                    AwaitingDimSectorSeperator, /**< After outside, awaiting the dimensional section */
                    AwaitingValSectorSeperator, /**< After the dimension section, awaiting the value section */ 
                    AwaitingDatSectorSeperator, /**< After the value section, awaiting the data section */
                    AwaitingAssignmentOperator, /**< After a variable name, awaiting a '=' */
                    AwaitingValue, /**< After a '=', awaiting a value literal */
                    AwaitingLeftParenthesis, /**< After a variable declaration, awaiting a '(' */
                    AwaitingDimensionName, /**< In dimension section, awaiting a name literal */ 
                    AwaitingMemberOperatorOrVariableName, /**< Multiple uses in variable section */
                    AwaitingMemberName, /**< Awaiting the name of an attribute */
                    Dimensions, /**< In dimension section */
                    Variables, /**< In variables section */
                    Data, /**< In data section */
                    End /**< End of file */
                };

                StreamPosition position = StreamPosition::Start; /**< The current state of the parser state machine */
                StreamPosition subPosition = StreamPosition::Start; /**< The current state of the subsection parser state machine */

                CDLDimension* currentDimension; /**< Pointer to the current dimension which is being build */
                ICDLVariable* currentVariable; /**< Pointer to the current (generic) variable which is being build */
                CDLAttribute* currentAttribute; /**< Pointer to the current attribute which is being build */

                Token lastLiteral; /**< The last literal parsed, used for saving the type of a variable declaration */
                bool globalAttribute = false; /**< Whether the current attribute is a global one */
                bool lineEmpty = false; /**< Whether the current line is now empty and the parser awaits a new section */
                bool isUnsigned = false; /**< Whether the current type is a unsigned one */

            };

            /**
             * @brief The current internal state of the parser
             */
            State state;

            /**
             * @brief Reference to the NetCDL data object which is being build
             */
            CDLData& data;

            /**
             * @brief Processes a token which is part of the dimensional subsection
             * 
             * @param token The token to be processed
             */
            void processDimensionalToken(Token token);

            /**
             * @brief Processes a token which is part of the variable subsection
             * 
             * @param token The token to be processed
             */
            void processVariableToken(Token token);

            /**
             * @brief Processes a token which is part of the data subsection
             * 
             * @param token The token to be processed
             */
            void processDataToken(Token token);

        public:

            /**
             * @brief Parses a CDL string to a NetCDF data object
             * 
             * @param[in] cdlString The CDL string
             * @param[out] data Reference to a NetCDF data object
             */
            static void CDLStringToData(string cdlString, CDLData& data);

            /**
             * @brief Parses a char stream into a NetCDF data object
             * 
             * @param[in] cdlStream Reference to a char stream
             * @param[out] data Reference to a NetCDF data object
             */  
            static void CDLStreamToData(istream& cdlStream, CDLData& data);

            CDLStreamParser(CDLData& data)
                : data(data)
            {};

            /**
             * @brief Processes a token and modifies the internal state
             * 
             * @param token The token to be processed
             */
            void processToken(Token token);

            /**
             * @brief Gets the actual NetCDF data object
             * 
             * @returns The NetCDF data object
             */
            CDLData retrieve() { return data; }

    };

}

#endif