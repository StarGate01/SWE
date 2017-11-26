#include "SWE_CdlParser.hh"

#include <stdexcept>
#include <cmath>

using namespace reader;

//TODO: Modify all implementations such, that they return the original string instead of the modified string when failing

bool SWE_CDLParser::readNextWord(string* text, string expected, const string seperators = " \n\t")
{
    //Find beginning of the word
    size_t start = (*text).find_first_not_of(seperators);
    //Return if string consists only of seperators
    if(start == string::npos)
        return false;

    //Cut front part of the string away
    *text = (*text).substr(start);

    //Return if impossible to find word in string
    if((*text).length < expected.length)
        return false;

    //Iterate though string and compare it to expected
    for(int i = 0; i < expected.length; i++)
    {
        if((*text)[i] != expected[i])
            return false;
    }

    //So far: Successful comparison
    //Cut string to remaining text
    *text = (*text).substr(expected.length);
    return true;
};

bool SWE_CDLParser::readIntAssignment(string* text, string var, const char op, int* ret, const string seperators = " ")
{
    //Read var
    if(!SWE_CDLParser::readNextWord(text, var, seperators))
        return false;       //Fail if invalid

    //Read op
    if(!SWE_CDLParser::readNextWord(text, string(1, op), seperators))
        return false;       //Fail if invalid

    try
    {
        (*ret) = readNextInt(text, seperators);
    }
    catch(const std::exception& e)
    {
        return false;
    }
    
    return true;
}

bool SWE_CDLParser::readDoubleAssignment(string* text, string var, const char op, double* ret, const string seperators = " ")
{
    //Read var
    if(!SWE_CDLParser::readNextWord(text, var, seperators))
        return false;       //Fail if invalid

    //Read op
    if(!SWE_CDLParser::readNextWord(text, string(1, op), seperators))
        return false;       //Fail if invalid

    try
    {
        (*ret) = readNextDouble(text, seperators);
    }
    catch(const std::exception& e)
    {
        return false;
    }

    return true;
}

int SWE_CDLParser::readNextInt(string* text, const string seperators = " ")
{
    //Find beginning of the number
    size_t start = (*text).find_first_not_of(seperators);

    //Return if string consists only of seperators
    if(start == string::npos)
        throw std::invalid_argument("No int value found");

    //Cut front part of the string away
    *text = (*text).substr(start);

    //Extract number until seperator
    size_t endofnumber = (*text).find_first_not_of("0123456789");
    string nrToParse = (*text).substr(0, endofnumber);

    //Convert and return number
    return std::stoi(nrToParse);
}

double SWE_CDLParser::readNextDouble(string* text, const string seperators = " ")
{
    //Find beginning of the number
    size_t start = (*text).find_first_not_of(seperators);

    //Return if string consists only of seperators
    if(start == string::npos)
        throw std::invalid_argument("No int value found");

    //Cut front part of the string away
    *text = (*text).substr(start);

    //Extract number until seperator
    size_t endofnumber = (*text).find_first_not_of("0123456789.");
    string nrToParse = (*text).substr(0, endofnumber);

    //Convert and return number
    return std::stod(nrToParse);
}

void SWE_CDLParser::parse_netCDF(string text)
{
    //Check header

    //extract name
    //Parse dimensions
    //Parse variables
    //Parse data
};