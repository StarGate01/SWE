#include "SWE_CdlParser.hh"

#include <stdexcept>
#include <cmath>

using namespace reader;

/**
 * @brief Reads the next word from a string, ignoring preceding 'seperator' characters and cuts the parsed section from the input string
 * 
 * This method iterates though the \p text string from beginning to end. It first skips all \p seperators characters, which are meant to be ignored in the parsing process.
 *  The following characters are compared to the \p expected string. If the characters match, the parsed section of the string is removed from \p text and true is returned.
 *  Otherwise the content of \p is not changed and false is returned.
 * 
 * @param text String to be parsed (will be changed)
 * @param expected Expected first word in the string
 * @param seperators Characters to be ignored at the start of the string
 * 
 * @return True if parsing was successful. Otherwise false
*/
bool SWE_CDLParser::readNextWord(string* text, string expected, const string seperators = " \n\t")
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

/**
 * @brief Parses a specific pattern of words, such as 'var = 12', in a string and cuts the parsed section from the input string
 * 
 * This method iterates though the \p text string from beginning to end. It first skips all \p seperators characters, which are meant to be ignored in the parsing process.
 *  It then expects a string followed by a specific char, seperated only by characters defined in \p seperators. The first string must be equal to \p var, then followed
 *  by a specific char defined in \p op. The third part must have the format of an int number and therefore only containing numbers. It will be parsed as int and returned in
 *  \ret.
 *  On success, the parsed section is removed from \p text. Otherwise, \text remains unchanged.
 *  Summary: A string of the form:
 *  <seperators>...<seperators><var><seperators>...<seperators><op><seperators>...<seperators><int number>
 *  can be parsed with this method.
 *  Example: 'truth = 42' can be parsed with \p var = "truth", \p op = '=' and \p seperators = " "
 * 
 * @param text String to be parsed (will be changed)
 * @param var First word
 * @param ret Pointer to number, where the result is stored
 * @param seperators Characters to be ignored at the start of the string
 * 
 * @return True if parsing was successful. Otherwise false
*/
bool SWE_CDLParser::readIntAssignment(string* text, string var, const char op, int* ret, const string seperators = " ")
{
    string originalString = *text;

    //Read var
    if(!SWE_CDLParser::readNextWord(text, var, seperators))
        return false;       //Fail if invalid

    //Read op
    if(!SWE_CDLParser::readNextWord(text, string(1, op), seperators))
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

/**
 * @brief Parses a specific pattern of words, such as 'var = 12.34', in a string and cuts the parsed section from the input string
 * 
 * This method iterates though the \p text string from beginning to end. It first skips all \p seperators characters, which are meant to be ignored in the parsing process.
 *  It then expects a string followed by a specific char, seperated only by characters defined in \p seperators. The first string must be equal to \p var, then followed
 *  by a specific char defined in \p op. The third part must have the format of a double number and therefore only containing numbers and dots. It will be parsed as double
 *  and returned in \p ret.
 *  On success, the parsed section is removed from \p text. Otherwise, \text remains unchanged.
 *  Summary: A string of the form:
 *  <seperators>...<seperators><var><seperators>...<seperators><op><seperators>...<seperators><double number>
 *  can be parsed with this method.
 *  Example: 'truth = 42.42' can be parsed with \p var = "truth", \p op = '=' and \p seperators = " "
 * 
 * @param text String to be parsed (will be changed)
 * @param var First word
 * @param ret Pointer to number, where the result is stored
 * @param seperators Characters to be ignored at the start of the string
 * 
 * @return True if parsing was successful. Otherwise false
*/
bool SWE_CDLParser::readDoubleAssignment(string* text, string var, const char op, double* ret, const string seperators = " ")
{
    string originalString = *text;

    //Read var
    if(!SWE_CDLParser::readNextWord(text, var, seperators))
        return false;       //Fail if invalid

    //Read op
    if(!SWE_CDLParser::readNextWord(text, string(1, op), seperators))
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

/**
 * @brief Parses an int number at the start of \p text, ignoring preceding \p seperators
 * 
 * First, characters defined in \p seperators are skipped. The following substring, parsed until the occurance of a seperator,
 *  is than parsed to an int value and returned.
 *  On success, the parsed section is removed from \p text. Otherwise, \text remains unchanged.
 *  
 * @param text String to be parsed (will be changed)
 * @param seperators Characters to be ignored at the start of the string
 * 
 * @return The parsed number
 * 
 * @throws Exceptions thrown by std::stoi, namely being std::invalid_argument and std::out_of_range
*/
int SWE_CDLParser::readNextInt(string* text, const string seperators = " ")
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

/**
 * @brief Parses a double number at the start of \p text, ignoring preceding \p seperators
 * 
 * First, characters defined in \p seperators are skipped. The following substring, parsed until the occurance of a seperator,
 *  is than parsed to a double value and returned.
 *  On success, the parsed section is removed from \p text. Otherwise, \text remains unchanged.
 *  
 * @param text String to be parsed (will be changed)
 * @param seperators Characters to be ignored at the start of the string
 * 
 * @return The parsed number
 * 
 * @throws Exceptions thrown by std::stod, namely being std::invalid_argument and std::out_of_range
*/
double SWE_CDLParser::readNextDouble(string* text, const string seperators = " ")
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

/**
 * @brief Parses a string at the start of \p text, ignoring preceding \p seperators
 * 
 * First, characters defined in \p seperators are skipped. The following substring, parsed until the occurance of a seperator,
 *  is than extracted and returned.
 *  On success, the parsed section is removed from \p text. Otherwise, \text remains unchanged.
 *  
 * @param text String to be parsed (will be changed)
 * @param seperators Characters to be ignored at the start of the string
 * 
 * @return The parsed string
*/
string SWE_CDLParser::readNextString(string* text, const string seperators = " ")
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

/**
 * @brief Parses a string at the start of \p text, ignoring preceding \p seperators
 * 
 * First, characters defined in \p seperators are skipped. The following substring, parsed until the occurance of a seperator,
 *  is than extracted and returned. \text remains unchanged.
 *  
 * @param text String to be parsed (will be changed)
 * @param seperators Characters to be ignored at the start of the string
 * 
 * @return The parsed string
*/
string SWE_CDLParser::peekNextString(string* text, const string seperators = " ")
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



void SWE_CDLParser::parse_netCDF(string text)
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
        throw std::runtime_error("Failed to find dimensions keyword");

    //TODO: Parse variables

    // ## Parse data ##
    if(!readNextWord(&text, "data:"))
        throw std::runtime_error("Failed to find dimensions keyword");

    //TODO: Parse data
};