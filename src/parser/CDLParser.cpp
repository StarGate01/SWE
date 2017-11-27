#include "CDLParser.hh"

#include <stdexcept>
#include <cmath>

//DEBUG
#include <iostream>

using namespace parser;


bool CDLParser::cutLeadingSeperators(string &text, string seperators)
{
    //Find beginning of the word
    size_t start = text.find_first_not_of(seperators);
    //Cut string if other content is found
    if(start != string::npos)
    {
        text = text.substr(start);
        return true;
    }
    text = "";
    return false;
};

std::list<Assignment<int>> CDLParser::readIntAssignmentList(string &text, const char op, string interSep, string finalSep, string seperators)
{
    std::list<Assignment<int>> lst;
    bool continueParsing = true;
    do
    {
        Assignment<int> item;
        //Peek variable name
        cutLeadingSeperators(text, seperators);
        item.name = peekNextString(text, seperators);

        //Read value
        if(!readIntAssignment(text, item.name, op, item.value, seperators))
        {
            throw std::invalid_argument("Failed to read int assignment");
        }

        //Store assignment
        lst.push_back(item);

        //Check for further items and remove interSep, if further items exist
        continueParsing = (peekNextString(text, seperators) == interSep);
        if(continueParsing)
            readNextString(text, seperators);
    }while(continueParsing);

    //Confirm final seperator
    if(!readNextWord(text, finalSep, seperators))
        throw std::invalid_argument("No final seperator detected at the end of list");

    return lst;
};

std::list<Assignment<double>> CDLParser::readDoubleAssignmentList(string &text, const char op, string interSep, string finalSep, string seperators)
{
    std::list<Assignment<double>> lst;
    bool continueParsing = true;
    do
    {
        Assignment<double> item;
        //Peek variable name
        cutLeadingSeperators(text, seperators);
        item.name = peekNextString(text, seperators);

        //Read value
        if(!readDoubleAssignment(text, item.name, op, item.value, seperators))
        {
            throw std::invalid_argument("Failed to read int assignment");
        }

        //Store assignment
        lst.push_back(item);

        //Check for further items and remove interSep, if further items exist
        continueParsing = (peekNextString(text, seperators) == interSep);
        if(continueParsing)
            readNextString(text, seperators);
    }while(continueParsing);

    //Confirm final seperator
    if(!readNextWord(text, finalSep, seperators))
        throw std::invalid_argument("No final seperator detected at the end of list");

    return lst;
};

bool CDLParser::readNextWord(string &text, string expected, string seperators)
{
    string originalString = text;

    //Cut leading seperators and check if it is still possible to find expected word
    if(!cutLeadingSeperators(text, seperators) || text.length() < expected.length())
    {
        text = originalString;
        return false;
    }

    //Iterate though string and compare it to expected
    for(int i = 0; i < (int) expected.length(); i++)
    {
        if(text[i] != expected[i])
        {
            text = originalString;
            return false;
        }
    }

    //Cut string to remaining text
    text = text.substr(expected.length());
    return true;
};

bool CDLParser::readNextWord(string &text, string expected)
{
    return readNextWord(text, expected, STRING_SEPERATOR);
};

bool CDLParser::readIntAssignment(string &text, string var, const char op, int &ret, string seperators)
{
    string originalString = text;

    //Read var and op
    if(!CDLParser::readNextWord(text, var, seperators) || !CDLParser::readNextWord(text, string(1, op), seperators))
    {
        text = originalString;
        return false;       //Fail if invalid
    }

    //Parse number
    try
    {
        ret = readNextInt(text, seperators);
    }
    catch(const std::exception& e)
    {
        text = originalString;
        return false;
    }
    
    return true;
};

bool CDLParser::readIntAssignment(string &text, string var, const char op, int &ret)
{
    return readIntAssignment(text, var, op, ret, STRING_SPACING);
};

bool CDLParser::readDoubleAssignment(string &text, string var, const char op, double &ret, string seperators)
{
    string originalString = text;

    //Read var and op
    if(!CDLParser::readNextWord(text, var, seperators) || !CDLParser::readNextWord(text, string(1, op), seperators))
    {
        text = originalString;
        return false;       //Fail if invalid
    }

    //Parse number
    try
    {
        ret = readNextDouble(text, seperators);
    }
    catch(const std::exception& e)
    {
        text = originalString;
        return false;
    }

    return true;
};

bool CDLParser::readDoubleAssignment(string &text, string var, const char op, double &ret)
{
    return readDoubleAssignment(text, var, op, ret, STRING_SPACING);
};

int CDLParser::readNextInt(string &text, string seperators)
{
    string originalString = text;

    if(!cutLeadingSeperators(text, seperators))
    {
        text = originalString;
        throw std::invalid_argument("No int value to parse");
    }

    //Extract number until seperator
    size_t endofnumber = text.find_first_not_of("0123456789");
    string nrToParse = text.substr(0, endofnumber);

    //Cut number away from text
    text = text.substr(endofnumber);

    //Convert and return number
    int res;
    try{
    res = std::stoi(nrToParse);
    }catch(std::exception& e)
    {
        text = originalString;
        throw std::invalid_argument("Failed to convert int: " + string(e.what()));
    }
    return res;
};

int CDLParser::readNextInt(string &text)
{
    return readNextInt(text, STRING_SPACING);
};

double CDLParser::readNextDouble(string &text, string seperators)
{
    string originalString = text;

    if(!cutLeadingSeperators(text, seperators))
    {
        text = originalString;
        throw std::invalid_argument("No double value to parse");
    }

    //Extract number until seperator
    size_t endofnumber = text.find_first_not_of("0123456789.");
    string nrToParse = text.substr(0, endofnumber);

    //Cut number away from text
    text = text.substr(endofnumber);

    double res;

    try{
        res = std::stod(nrToParse);
    }catch(std::exception& e)
    {
        text = originalString;
        throw std::invalid_argument("No double value found");
    }
    //Convert and return number
    return res;
};

double CDLParser::readNextDouble(string &text)
{
    return readNextDouble(text, STRING_SPACING);
};

string CDLParser::readNextString(string &text, string seperators)
{
    string originalString = text;

    if(!cutLeadingSeperators(text, seperators))
    {
        text = originalString;
        throw std::invalid_argument("No int value to parse");
    }

    //Extract string until seperator
    size_t endofstring = text.find_first_of(seperators);
    string str = text.substr(0, endofstring);

    //Ct word away the parsed section from string
    text = text.substr(endofstring);

    //Return string
    return str;
};

string CDLParser::readNextString(string &text)
{
    return readNextString(text, STRING_SPACING);
};

string CDLParser::peekNextString(string &text, string seperators)
{
    string copy = text;

    //Remove leading seperators in string
    if(!cutLeadingSeperators(copy, seperators))
    {
        text = copy;
        throw std::invalid_argument("Only seperators found in text");
    }
    //Extract string until seperator
    size_t endofstring = copy.find_first_of(seperators);
    string str = copy.substr(0, endofstring);

    //Convert and return number
    return str;
};

string CDLParser::peekNextString(string &text)
{
    return peekNextString(text, STRING_SPACING);
};

void CDLParser::parse_netCDF(string text)
{
    // ## Check header ##
    if(!readNextWord(text, "netcdf"))
        throw std::runtime_error("Failed to parse header");

    // ## extract name ##
    string scenario_name = readNextString(text, " ");

    //TODO: With the use of peekNextString(), the order of 'dimensions', 'variables' and 'data' could be random... parse whatever comes first

    // ## Parse dimensions ##
    if(!readNextWord(text, "dimensions:"))
        throw std::runtime_error("Failed to find dimensions keyword");
    
    //TODO: Parse dimensions

    // ## Parse variables ##
    if(!readNextWord(text, "variables:"))
        throw std::runtime_error("Failed to find variables keyword");

    //TODO: Parse variables

    // ## Parse data ##
    if(!readNextWord(text, "data:"))
        throw std::runtime_error("Failed to find data keyword");

    //TODO: Parse data
};