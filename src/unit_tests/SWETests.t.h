#include <cxxtest/TestSuite.h>
#include "../parser/CDLParser.hh"
#include "../../submodules/solvers/src/solver/FConst.hpp"       //Required for ZERO_PRECISION

namespace swe_tests
{
    class SWETestsSuite;
}

class swe_tests::SWETestsSuite : public CxxTest::TestSuite
{
    public:

        /**
         * @test Verify implementation of parser::CDLParser::readNextString
        */
        void testReadNextString(void)
        {
            // ## Valid test ##
            string* text = new string("That's one small step for a man, one giant leap for mankind");
            
            TS_ASSERT_EQUALS(parser::CDLParser::readNextString(text).compare("That\'s"), 0);
            TS_ASSERT_EQUALS(text->compare(" one small step for a man, one giant leap for mankind"), 0);
            
            // ## Invalid text ##
            (*text) = "    ";
            bool exceptionThrown = false;
            try{
                parser::CDLParser::readNextString(text);
            }
            catch(exception& e)
            {
                exceptionThrown = true;
            }
            TS_ASSERT(exceptionThrown);
        }

    /**
     * @test Verify implementation of parser::CDLParser::peekNextString
    */
    void testPeekNextString(void)
    {
        const string str = "  secret ";
        string* text = new string(str);
        
        // ## Valid test ##
        TS_ASSERT_EQUALS(parser::CDLParser::peekNextString(text, " ").compare("secret"), 0);
        TS_ASSERT_EQUALS(text->compare(str), 0);

        // ## Invalid test ##
        bool exceptionThrown = false;
        (*text) = string("   ");
        try{
            string result = parser::CDLParser::peekNextString(text);
        }
        catch(exception& e)
        {
            exceptionThrown = true;
        }
        TS_ASSERT(exceptionThrown);
    }

    //TODO: Debug this test
    //TODO: Add documentation
    void testReadAssignment(void)
    {
        int reti;
        double retd;
        
        string* text = new string("  a = 123  b = 123.45 charlie = notreallyavalidnumber");
        // ## Test int (valid) ##
        TS_ASSERT(parser::CDLParser::readIntAssignment(text, string("a"), '=',  reti));
        //Test value is as expected
        TS_ASSERT_EQUALS(reti, 123);
        //Test assignment removed from text
        TS_ASSERT((*text) == "  b = 123.45 charlie = notreallyavalidnumber");

        // ## Test double (valid) ##
        TS_ASSERT(parser::CDLParser::readDoubleAssignment(text, string("b"), '=', retd));
        //Test value is as expected
        TS_ASSERT_DELTA(retd, 123.45, ZERO_PRECISION);
        //Test assignment removed from text
        TS_ASSERT((*text) == " charlie = notreallyavalidnumber");

        // ## Test int (invalid): value is not a number ##
        TS_ASSERT(!parser::CDLParser::readIntAssignment(text, string("charlie"), '=', reti));
        //Test text remains equal
        TS_ASSERT((*text) == " charlie = notreallyavalidnumber");
          
        // ## Test int (invalid): variable name is invalid ##
        TS_ASSERT(!parser::CDLParser::readIntAssignment(text, string("november"), '=', reti));
        //Test text remains equal
        TS_ASSERT((*text) == " charlie = notreallyavalidnumber");

        // ## Test int (invalid): operator is invalid ##
        TS_ASSERT(!parser::CDLParser::readIntAssignment(text, string("charlie"), '#', reti));
        //Test text remains equal
        TS_ASSERT((*text) == " charlie = notreallyavalidnumber");

        // ## Test double (invalid) ##
        TS_ASSERT(!parser::CDLParser::readDoubleAssignment(text, string("charlie"), '=', retd));
        //Test text remains equal
        TS_ASSERT((*text) == " charlie = notreallyavalidnumber");

        // ## Test double (invalid): variable name is invalid ##
        TS_ASSERT(!parser::CDLParser::readDoubleAssignment(text, string("november"), '=', retd));
        //Test text remains equal
        TS_ASSERT((*text) == " charlie = notreallyavalidnumber");

        // ## Test double (invalid): operator is invalid ##
        TS_ASSERT(!parser::CDLParser::readDoubleAssignment(text, string("charlie"), '#', retd));
        //Test text remains equal
        TS_ASSERT((*text) == " charlie = notreallyavalidnumber");
    }

    //TODO: Add documentation
    void testReadNextWord(void)
    {
        string* text = new string("This      is\na random text\tto test some bug-free methods");

        // ## Test 1 ##
        //Read valid word at beginning of text
        TS_ASSERT(parser::CDLParser::readNextWord(text, string("This")));
        //Check First word is removed from text
        TS_ASSERT((*text) == "      is\na random text\tto test some bug-free methods");

        // ## Test 2 ##
        //Read valid word after multiple seperators
        TS_ASSERT(parser::CDLParser::readNextWord(text, string("is")));
        //Check word and preceding seperators are removed from text
        TS_ASSERT((*text) == "\na random text\tto test some bug-free methods");

        // ## Test 3 ##
        //Read multiple valid words after seperator containing a seperator
        TS_ASSERT(parser::CDLParser::readNextWord(text, string("a random text")));
        //Check word and preceding seperators are removed from text
        TS_ASSERT((*text) == "\tto test some bug-free methods");

        // ## Test 4 ##
        //Read invalid text (must fail)
        TS_ASSERT(parser::CDLParser::readNextWord(text, string("invalid")) == false);
        //Check text remains unchanged
        TS_ASSERT((*text) == "\tto test some bug-free methods");
    };
};
