#include "SWE_CdlParser.hh"
#include <cmath>

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

void SWE_CDLParser::parse_netCDF(string text)
{
    //Check header
    //extract name
    //Parse dimensions
    //Parse variables
    //Parse data
};