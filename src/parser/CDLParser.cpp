#include "CDLParser.hh"

#include <stdexcept>
#include <cmath>

using namespace reader;


bool CDLParser::readNextWord(string* text, string expected, const string seperators = " \n\t")
{
    string originalString = *text;

    //Find beginning of the word
    size_t start = (*text).find_first_not_of(seperators);
    //Return if string consists only of seperators
    if(start == string::npos)
    {
        *text = originalString.copy;
        return false;
    }

    //Cut front part of the string away
    *text = (*text).substr(start);

    //Return if impossible to find word in string
    if((*text).length < expected.length)
    {
        *text = originalString.copy;
        return false;
    }

    //Iterate though string and compare it to expected
    for(int i = 0; i < expected.length; i++)
    {
        if((*text)[i] != expected[i])
        {
            *text = originalString.copy;
            return false;
        }
    }

    //So far: Successful comparison
    //Cut string to remaining text
    *text = (*text).substr(expected.length);
    return true;
};

bool CDLParser::readIntAssignment(string* text, string var, const char op, int* ret, const string seperators = " ")
{
    string originalString = *text;

    //Read var
    if(!CDLParser::readNextWord(text, var, seperators))
        return false;       //Fail if invalid

    //Read op
    if(!CDLParser::readNextWord(text, string(1, op), seperators))
        return false;       //Fail if invalid

    //Parse number
    try
    {
        (*ret) = readNextInt(text, seperators);
    }
    catch(const std::exception& e)
    {
        *text = originalString;
        return false;
    }
    
    return true;
}

bool CDLParser::readDoubleAssignment(string* text, string var, const char op, double* ret, const string seperators = " ")
{
    string originalString = *text;

    //Read var
    if(!CDLParser::readNextWord(text, var, seperators))
        return false;       //Fail if invalid

    //Read op
    if(!CDLParser::readNextWord(text, string(1, op), seperators))
        return false;       //Fail if invalid

    //Parse number
    try
    {
        (*ret) = readNextDouble(text, seperators);
    }
    catch(const std::exception& e)
    {
        *text = originalString;
        return false;
    }

    return true;
}

int CDLParser::readNextInt(string* text, const string seperators = " ")
{
    string originalString = *text;

    //Find beginning of the number
    size_t start = (*text).find_first_not_of(seperators);

    //Return if string consists only of seperators
    if(start == string::npos)
    {
        *text = originalString;
        throw std::invalid_argument("No int value found");
    }
        
    //Cut front part of the string away
    *text = (*text).substr(start);

    //Extract number until seperator
    size_t endofnumber = (*text).find_first_not_of("0123456789");
    string nrToParse = (*text).substr(0, endofnumber);

    //Convert and return number
    return std::stoi(nrToParse);
}

double CDLParser::readNextDouble(string* text, const string seperators = " ")
{
    string originalString = *text;

    //Find beginning of the number
    size_t start = (*text).find_first_not_of(seperators);

    //Return if string consists only of seperators
    if(start == string::npos)
    {
        *text = originalString;
        throw std::invalid_argument("No int value found");
    }

    //Cut front part of the string away
    *text = (*text).substr(start);

    //Extract number until seperator
    size_t endofnumber = (*text).find_first_not_of("0123456789.");
    string nrToParse = (*text).substr(0, endofnumber);

    //Convert and return number
    return std::stod(nrToParse);
}

string CDLParser::readNextString(string* text, const string seperators = " ")
{
    string originalString = *text;

    //Find beginning of the string
    size_t start = (*text).find_first_not_of(seperators);

    //Return if string consists only of seperators
    if(start == string::npos)
    {
        *text = originalString;
        throw std::invalid_argument("No int value found");
    }

    //Cut front part of the string away
    *text = (*text).substr(start);

    //Extract string until seperator
    size_t endofstring = (*text).find_first_of(seperators);
    string str = (*text).substr(0, endofstring);

    //Return string
    return str;
}

string CDLParser::peekNextString(string* text, const string seperators = " ")
{
    string originalString = *text;

    //Find beginning of the string
    size_t start = originalString.find_first_not_of(seperators);

    //Return if string consists only of seperators
    if(start == string::npos)
        throw std::invalid_argument("No int value found");

    //Cut front part of the string away
    originalString = originalString.substr(start);

    //Extract string until seperator
    size_t endofstring = originalString.find_first_of(seperators);
    string str = originalString.substr(0, endofstring);

    //Convert and return number
    return str;
}



void CDLParser::parse_netCDF(string text)
{
    // ## Check header ##
    if(!readNextWord(&text, "netcdf"))
        throw std::runtime_error("Failed to parse header");

    // ## extract name ##
    string scenario_name = readNextString(&text, " ");

    //TODO: With the use of peekNextString(), the order of 'dimensions', 'variables' and 'data' could be random... parse whatever comes first

    // ## Parse dimensions ##
    if(!readNextWord(&text, "dimensions:"))
        throw std::runtime_error("Failed to find dimensions keyword");
    
    //TODO: Parse dimensions

    // ## Parse variables ##
    if(!readNextWord(&text, "variables:"))
        throw std::runtime_error("Failed to find variables keyword");

    //TODO: Parse variables

    // ## Parse data ##
    if(!readNextWord(&text, "data:"))
        throw std::runtime_error("Failed to find data keyword");

    //TODO: Parse data
};