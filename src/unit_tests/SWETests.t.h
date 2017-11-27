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
        bool compareIntAssignment(std::list<parser::Assignment<int>> a, std::list<parser::Assignment<int>> b)
        {
            //Check number of items in list equal
            if(a.size() != b.size())
            {
                std::cout << "Failed to compare size a=" << a.size() << " != " << b.size() << std::endl;
                return false;
            }
            
            std::list<parser::Assignment<int>>::iterator a_it;
            std::list<parser::Assignment<int>>::iterator b_it;
            for(a_it = a.begin(), b_it = b.begin(); a_it != a.end() && b_it != b.end(); a_it++, b_it++)
            {
                if(a_it->name != b_it->name || a_it->value != b_it->value)
                {
                    std::cout << std::endl << "Failed to compare item: " << a_it->name << "=" << b_it->name << " | " << a_it->value << "=" << b_it->value << std::endl;
                    return false;
                }
            }
            return true;
        }

        bool compareDoubleAssignment(std::list<parser::Assignment<double>> a, std::list<parser::Assignment<double>> b)
        {
            //Check number of items in list equal
            if(a.size() != b.size())
            {
                std::cout << "Failed to compare size a=" << a.size() << " != " << b.size() << std::endl;
                return false;
            }
            
            std::list<parser::Assignment<double>>::iterator a_it;
            std::list<parser::Assignment<double>>::iterator b_it;
            for(a_it = a.begin(), b_it = b.begin(); a_it != a.end() && b_it != b.end(); a_it++, b_it++)
            {
                if(a_it->name != b_it->name || a_it->value != b_it->value)
                {
                    std::cout << std::endl << "Failed to compare item: " << a_it->name << "=" << b_it->name << " | " << a_it->value << "=" << b_it->value << std::endl;
                    return false;
                }
            }
            return true;
        }

        void testReadIntAssignmentList(void)
        {
            // ## Test 1 (int) ##
            string text = "first = 11, second = 12, third = 13;";
            std::list<parser::Assignment<int>> ret = parser::CDLParser::readIntAssignmentList(text, '=', ",", ";", " ");

            struct parser::Assignment<int> i1 = {"first", 11};
            struct parser::Assignment<int> i2 = {"second", 12};
            struct parser::Assignment<int> i3 = {"third", 13};
            std::list<parser::Assignment<int>> exp = {i1, i2, i3};          
            TS_ASSERT(compareIntAssignment(ret, exp));


            // ## Test 2 (double) ##
            text = "first = 1.1, second = 1.2, third = 1.3;";
            std::list<parser::Assignment<double>> retd = parser::CDLParser::readDoubleAssignmentList(text, '=', ",", ";", " ");

            struct parser::Assignment<double> d1 = {"first", 1.1};
            struct parser::Assignment<double> d2 = {"second", 1.2};
            struct parser::Assignment<double> d3 = {"third", 1.3};
            std::list<parser::Assignment<double>> expd = {d1, d2, d3};          
            TS_ASSERT(compareDoubleAssignment(retd, expd));
        }

        /**
         * @test Verify implementation of parser::CDLParser::readNextString
        */
        void testReadNextString(void)
        {
            // ## Valid test ##
            string text = string("That's one small step for a man, one giant leap for mankind");
            
            TS_ASSERT_EQUALS(parser::CDLParser::readNextString(text).compare("That\'s"), 0);
            TS_ASSERT_EQUALS(text.compare(" one small step for a man, one giant leap for mankind"), 0);
            
            // ## Invalid text ##
            text = "    ";
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
        string text = string(str);
        
        // ## Valid test ##
        TS_ASSERT_EQUALS(parser::CDLParser::peekNextString(text, " ").compare("secret"), 0);
        TS_ASSERT_EQUALS(text.compare(str), 0);

        // ## Invalid test ##
        bool exceptionThrown = false;
        text = string("   ");
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
        
        string text = string("  a = 123  b = 123.45 charlie = notreallyavalidnumber");
        // ## Test int (valid) ##
        TS_ASSERT(parser::CDLParser::readIntAssignment(text, string("a"), '=',  reti));
        //Test value is as expected
        TS_ASSERT_EQUALS(reti, 123);
        //Test assignment removed from text
        TS_ASSERT(text == "  b = 123.45 charlie = notreallyavalidnumber");

        // ## Test double (valid) ##
        TS_ASSERT(parser::CDLParser::readDoubleAssignment(text, string("b"), '=', retd));
        //Test value is as expected
        TS_ASSERT_DELTA(retd, 123.45, ZERO_PRECISION);
        //Test assignment removed from text
        TS_ASSERT(text == " charlie = notreallyavalidnumber");

        // ## Test int (invalid): value is not a number ##
        TS_ASSERT(!parser::CDLParser::readIntAssignment(text, string("charlie"), '=', reti));
        //Test text remains equal
        TS_ASSERT(text == " charlie = notreallyavalidnumber");
          
        // ## Test int (invalid): variable name is invalid ##
        TS_ASSERT(!parser::CDLParser::readIntAssignment(text, string("november"), '=', reti));
        //Test text remains equal
        TS_ASSERT(text == " charlie = notreallyavalidnumber");

        // ## Test int (invalid): operator is invalid ##
        TS_ASSERT(!parser::CDLParser::readIntAssignment(text, string("charlie"), '#', reti));
        //Test text remains equal
        TS_ASSERT(text == " charlie = notreallyavalidnumber");

        // ## Test double (invalid) ##
        TS_ASSERT(!parser::CDLParser::readDoubleAssignment(text, string("charlie"), '=', retd));
        //Test text remains equal
        TS_ASSERT(text == " charlie = notreallyavalidnumber");

        // ## Test double (invalid): variable name is invalid ##
        TS_ASSERT(!parser::CDLParser::readDoubleAssignment(text, string("november"), '=', retd));
        //Test text remains equal
        TS_ASSERT(text == " charlie = notreallyavalidnumber");

        // ## Test double (invalid): operator is invalid ##
        TS_ASSERT(!parser::CDLParser::readDoubleAssignment(text, string("charlie"), '#', retd));
        //Test text remains equal
        TS_ASSERT(text == " charlie = notreallyavalidnumber");
    }

    //TODO: Add documentation
    void testReadNextWord(void)
    {
        string text = string("This      is\na random text\tto test some bug-free methods");

        // ## Test 1 ##
        //Read valid word at beginning of text
        TS_ASSERT(parser::CDLParser::readNextWord(text, string("This")));
        //Check First word is removed from text
        TS_ASSERT(text == "      is\na random text\tto test some bug-free methods");

        // ## Test 2 ##
        //Read valid word after multiple seperators
        TS_ASSERT(parser::CDLParser::readNextWord(text, string("is")));
        //Check word and preceding seperators are removed from text
        TS_ASSERT(text == "\na random text\tto test some bug-free methods");

        // ## Test 3 ##
        //Read multiple valid words after seperator containing a seperator
        TS_ASSERT(parser::CDLParser::readNextWord(text, string("a random text")));
        //Check word and preceding seperators are removed from text
        TS_ASSERT(text == "\tto test some bug-free methods");

        // ## Test 4 ##
        //Read invalid text (must fail)
        TS_ASSERT(parser::CDLParser::readNextWord(text, string("invalid")) == false);
        //Check text remains unchanged
        TS_ASSERT(text == "\tto test some bug-free methods");
    };
};
