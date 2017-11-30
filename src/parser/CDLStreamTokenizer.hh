/**
 * @file CDLStreamTokenizer.hh
 * @brief Provides a tokenizer for CDL strings
 */

#ifndef SWE_CDLSTREAMTOKENIZER_HH
#define SWE_CDLSTREAMTOKENIZER_HH

#include <string>
#include <queue>

using namespace std;

namespace parser
{

    /**
     * @brief Represents a token to be used by CDLStreamParser
     */
    struct Token
    {

        /**
         * @brief The possible types of tokens
         */
        enum class Type : unsigned int
        { 
            None, /**< Used for internal management only */
            Literal, /**< A name, value or generally everything which is not a control char */
            Seperator, /**< The line seperator ';' */
            DataSeperator, /**< The data seperator ',' */
            SectorSeperator, /**< The sector seperator ':\\n' */
            AssignmentOperator, /**< The assignment operator '=' */
            MemberOperator, /**< The member access operator ':' */
            LeftParenthesis, /**< A opening parenthesis '(' */
            RightParenthesis, /**< A closing parenthesis ')' */
            LeftBrace, /**< A opening brace '{' */
            RightBrace /**< A closing brace '}' */
        };

        Type type; /**< The token type */
        string value; /**< The token value */

        Token()
        {};

        /**
         * @brief Constructs a token with initial values
         * 
         * @param type The type
         * @param value The value
         */
        Token(Type type, string value)
            : type(type),
              value(value)
        {};

    };

    /**
     * @brief Tokenizer for CDL strings
     */
    class CDLStreamTokenizer
    {

        private:
            
            /**
             * @brief The internal state of CDLStreamTokenizer
             */
            struct State
            {

                string literalBuffer = ""; /**< Buffer for literals */
                bool hadColon = false; /**< Whether The last line ended with a colon */
                char commentState = 0; /**< How many forward slashes were found in series */
                bool verbatim = false; /**< Whether there is currently a verbatim string being scanned */

            };

            /** 
             * @brief The current state of the tokenizer state machineThe current state of the tokenizer state machine
             */
            State state;

            /**
             * @brief The queue of currently available tokens
             */
            queue<Token> tokens;

        public:

            CDLStreamTokenizer() 
            {};

            /**
             * @brief Whether there are tokens in the queue
             * 
             * @returns Whether there are tokens in the queue
             */
            bool hasTokens();

            /**
             * @brief Gets the oldest token in the queue
             * 
             * @returns The oldest token
             */
            Token getToken();

            /**
             * @brief Processes a char and modifies the internal state
             * 
             * @param character The char to be read
             */
            void read(char character);

    };

}

#endif