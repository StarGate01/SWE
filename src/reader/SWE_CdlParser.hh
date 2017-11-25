#ifndef SWE_CDLPARSER_HH
#define SWE_CDLPARSER_HH

#include <string>

using namespace std;

namespace reader
{

    class SWE_CDLParser
    {

    private:
        //TODO: Implement data structure of parsed data

    public:
        static bool readNextWord(string* text, string expected, const string seperators = " \n\t");
        static bool readIntAssignment(string* text, string var, const char op, int* ret, const string seperators = " ");
        static bool readDoubleAssignment(string* text, string var, const char op, double* ret, const string seperators = " ");
        static int readNextInt(string* text, const string seperators = " ");    
        static double readNextDouble(string* text, const string seperators = " ");
        void parse_netCDF(string text);

    };

}

#endif