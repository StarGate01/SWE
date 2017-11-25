#ifndef SWE_CDLPARSER_HH
#define SWE_CDLPARSER_HH

#include <string>

using namespace std;

class SWE_CDLParser{
private:
    //TODO: Implement data structure of parsed data
public:
    static bool readNextWord(string* text, string expected, string seperators = " \n\t");
    void parse_netCDF(string text);
};

#endif