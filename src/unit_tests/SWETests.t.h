#include <cxxtest/TestSuite.h>
#include "../SWE_CdlParser.hh"

namespace swe_tests
{
    class SWETestsSuite;
}

class swe_tests::SWETestsSuite : public CxxTest::TestSuite
{
    //TODO: Debug this test
    void testReadAssignment(void)
    {
        string* text = new string("  a = 123  b = 123.45 charlie = notreallyavalidnumber");
        // ## Test int (valid) ##
        TS_ASSERT(SWE_CDLParser::readIntAssignment(text, string("a"), '='));
        //Test assignment removed from text
        TS_ASSERT((*text) == "  b = 123.45 charlie = notreallyavalidnumber");

        // ## Test double (valid) ##
        TS_ASSERT(SWE_CDLParser::readDoubleAssignment(text, string("b"), '='));
        //Test assignment removed from text
        TS_ASSERT((*text) == " charlie = notreallyavalidnumber");

        // ## Test int (invalid): value is not a number ##
        TS_ASSERT(SWE_CDLParser::readIntAssignment(text, string("charlie"), '='));
        //Test text remains equal
        TS_ASSERT((*text) == " charlie = notreallyavalidnumber");

        // ## Test int (invalid): variable name is invalid ##
        TS_ASSERT(SWE_CDLParser::readIntAssignment(text, string("november"), '='));
        //Test text remains equal
        TS_ASSERT((*text) == " charlie = notreallyavalidnumber");

        // ## Test int (invalid): operator is invalid ##
        TS_ASSERT(SWE_CDLParser::readIntAssignment(text, string("charlie"), '#'));
        //Test text remains equal
        TS_ASSERT((*text) == " charlie = notreallyavalidnumber");

        // ## Test double (invalid) ##
        TS_ASSERT(SWE_CDLParser::readDoubleAssignment(text, string("charlie"), '='));
        //Test text remains equal
        TS_ASSERT((*text) == " charlie = notreallyavalidnumber");

        // ## Test double (invalid): variable name is invalid ##
        TS_ASSERT(SWE_CDLParser::readDoubleAssignment(text, string("november"), '='));
        //Test text remains equal
        TS_ASSERT((*text) == " charlie = notreallyavalidnumber");

        // ## Test double (invalid): operator is invalid ##
        TS_ASSERT(SWE_CDLParser::readDoubleAssignment(text, string("charlie"), '#'));
        //Test text remains equal
        TS_ASSERT((*text) == " charlie = notreallyavalidnumber");
    }

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
        //Read invalid text (must fail)
        TS_ASSERT(SWE_CDLParser::readNextWord(text, string("invalid")) == false);
        //Check text remains unchanged
        TS_ASSERT((*text) == "\tto test some bug-free methods");
    };
};
