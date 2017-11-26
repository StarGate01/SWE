#ifndef SWE_CDLPARSER_HH
#define SWE_CDLPARSER_HH

#include <string>

using namespace std;

namespace reader
{

    class CDLParser
    {

    private:
        //TODO: Implement data structure of parsed data

    public:
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
        static bool readNextWord(string* text, string expected, const string seperators = " \n\t");

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
        static bool readIntAssignment(string* text, string var, const char op, int* ret, const string seperators = " ");

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
        static bool readDoubleAssignment(string* text, string var, const char op, double* ret, const string seperators = " ");

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
        static int readNextInt(string* text, const string seperators = " ");    

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
        static double readNextDouble(string* text, const string seperators = " ");

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
         * 
         * @throws std::invalid_argument, if \p text contains only seperators
        */
        static string readNextString(string* text, const string seperators = " ");

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
         * 
         * @throws std::invalid_argument, if \p text contains only seperators
        */
        static string peekNextString(string* text, const string seperators = " ");
        
        void parse_netCDF(string text);

    };

}

#endif