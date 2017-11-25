#include <cxxtest/TestSuite.h>
#include "../SWE_CdlParser.hh"

namespace swe_tests
{
    class SWETestsSuite;
}

class swe_tests::SWETestsSuite : public CxxTest::TestSuite
{
    void testReadNextWord(void)
    {
        string* text = new string("This      is\na random text\tto test some bug-free methods");

        // ## Test 1 ##
        //Read valid word at beginning of text
        TS_ASSERT(SWE_CDLParser::readNextWord(text, string("This")));
        //Check First word is removed from text
        TS_ASSERT((*text) == "      is\na random text\tto test some bug-free methods");

        // ## Test 2 ##
        //Read valid word after multiple seperators
        TS_ASSERT(SWE_CDLParser::readNextWord(text, string("is")));
        //Check word and preceding seperators are removed from text
        TS_ASSERT((*text) == "\na random text\tto test some bug-free methods");

        // ## Test 3 ##
        //Read multiple valid words after seperator containing a seperator
        TS_ASSERT(SWE_CDLParser::readNextWord(text, string("\na random text")));
        //Check word and preceding seperators are removed from text
        TS_ASSERT((*text) == "\tto test some bug-free methods");

        // ## Test 4 ##
        //Read invalid text
        TS_ASSERT(SWE_CDLParser::readNextWord(text, string("invalid")));
        //Check text remains unchanged
        TS_ASSERT((*text) == "\tto test some bug-free methods");
    };
};
