#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <map>
#include <list>
#include <vector>
#include "mud.h"
#include "conf.h"
#include "editor.h"

//misc
inline BOOL IsAlpha(const char* const character);
BOOL FileExists(const std::string &name);
/*
*Tokenizes the string into chunks based on the delimiter.
*Param: [in] the string to tokenize.
*[out] a vector of strings, where the parts will go.
*param: [in] The delimiter.
*Return: void
*note: The slices will be put into tokens. If no tokens are found, the vector will contain the string passed in.
*/
void Tokenize(const std::string &str, std::vector<std::string> &tokens, const std::string &del=" ");
//player validation functions
BOOL IsValidUserName(const std::string &input);
BOOL IsValidPassword(const std::string &input);
BOOL PlayerExists(const std::string &name);
BOOL IsFirstUser();
//string functions:
void Lower(std::string &str);
std::string Capitalize(const std::string &str);
std::string Repete(const std::string &filler,const int count);
std::string Center(const std::string &str,const int width);
std::string Explode(std::vector <std::string> &parts, const std::string &del = " ");
std::string StripWhitespace(const std::string &str);

//bitfield manipulation
FLAG BitSet(FLAG flag,int pos);
FLAG BitClear(FLAG flag,int pos);
BOOL BitIsSet(FLAG flag,int pos);
//minmax
template <typename T>
inline T Min(const T a,const T b);
template <typename T>
inline T Max(const T a,const T b);
/*
*converts a list of strings to an english list (a, b, c, and d).
*Param: [in] a pointer to a list of std::string.
*Return: the string converted.
*/
std::string EnglishList(std::list <std::string> *in);
/*
*Simulates dice rolls, using x number of dice with a set number of sides.
*Param: [in] the number of dice.
*[in] the number of sides.
*Return: the sum of the result.
*/
int Roll(int n,int s);
/*
*Gets the name of the opposite exit.
*Param: [in] the name of the exit.
*Return: The opposite exit from the one specified, or "" if the exit was unknown.
*For example, if you provide north, the exit will be south, east will be west, etc.
*/
std::string SwapExit(const std::string &exit);
/*
*Checks to see if the exit name given is a valid exit.
*Param: [in] the name or alias of the exit.\
*return: true on success, false on failure.
*/
BOOL IsValidExit(const std::string &name);
/*
*Returns the exit direction associated with each exit.
*Param: [in] the name of the exit.
*return: The number associated with that direction.
*/
ExitDirection GetDirectionByName(const std::string &name);
/*
*Returns a string with the inputted list of data columnized
*Param: [in] a vector of strings with the data to columnize.
*[in] The number of columns to create.
*[in] The max width of the columns.
*Return: A string with the columnized data.
*/
std::string Columnize(std::vector<std::string> *data, int cols, int width=80);
//time functions:
class TimeInfo
{
    int hour;
    int minute;
    int second;

    void Initialize(void);
public:
    TimeInfo(time_t data);
    TimeInfo(UINT data);
    TimeInfo();
    void Calculate(time_t data);
    std::string ToString(void);
};
#endif
