/*
*utils.h
*
*   Copyright 2010 Tyler Littlefield.
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*/


#ifndef HEADER_UTILS
#define HEADER_UTILS
#include <string>
#include <map>
#include <list>
#include <vector>
#include "mud.h"
#include "conf.h"
#include "serializer.hpp"

//misc
inline BOOL IsAlpha(const char* const character);
BOOL FileExists(const std::string &name);
//parsing
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
std::string Explode(std::vector <std::string> &parts);
std::string StripWhitespace(const std::string &str);

//bitfield manipulation
FLAG BitSet(FLAG flag,int pos);
FLAG BitClear(FLAG flag,int pos);
BOOL BitIsSet(FLAG flag,int pos);
//minmax
template <typename _v>
BOOL Min(const _v a,const _v b);
template <typename _v>
BOOL Max(const _v a,const _v b);
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

template <typename T>
inline void SerializeDictionary(Serializer& s, std::map<std::string, T > &dict)
{
    typename std::map <std::string, T>::iterator it;
    unsigned int size=dict.size();

    s << size;
    if (size) {
        for (it=dict.begin(); it!=dict.end(); it++) {
            s << (*it).first;
            s << (*it).second;
        }
    }
}
template <typename T>
inline void DeserializeDictionary(Serializer& s, std::map<std::string, T> &dict)
{
    unsigned int size;
    unsigned int i;

    std::string key;
    T val;

    s >> size;
    if (size) {
        for (i=0; i<size; i++) {
            s >> key;
            s >> val;

            dict[key]=val;
        }
    }
}

template <typename T>
inline void SerializeObjects(Serializer& s, std::map<std::string, T*> &dict)
{
    typename std::map <std::string, T*>::iterator it;
    unsigned int size=dict.size();

    s << size;
    if (size) {
        for (it=dict.begin(); it!=dict.end(); it++) {
            s << (*it).first;
            (*it).second->serialize(s);
        }
    }
}
template <typename T>
inline void DeserializeObjects(Serializer& s, std::map<std::string, T*> &dict)
{
    std::string key;
    T* val;
    unsigned int size, i;

    s >> size;
    if (size) {
        for (i=0; i<size; i++) {
            s >> key;
            val = new T();
            val->Deserialize(s);
            dict[key]=val;
        }
    }
}
template <typename T>
inline void SerializeObjects(Serializer& s, std::map<std::string, T> &dict)
{
    typename std::map <std::string, T*>::iterator it;
    unsigned int size=dict.size();

    s << size;
    if (size) {
        for (it=dict.begin(); it!=dict.end(); it++) {
            s << (*it).first;
            (*it).second.serialize(s);
        }
    }
}
template <typename T>
inline void DeserializeObjects(Serializer& s, std::map<std::string, T> &dict)
{
    std::string key;
    T val;
    unsigned int size, i;

    s >> size;
    if (size) {
        for (i=0; i<size; i++) {
            s >> key;
            val.Deserialize(s);
            dict[key]=val;
        }
    }
}
template <typename T>
inline void SerializeObjectList(Serializer& s, std::list <T*> &olist)
{
    typename std::list<T*>::iterator it;
    UINT size=olist.size();
    s << size;

    if (size) {
        for (it=olist.begin(); it!=olist.end(); it++) {
            (*it)->Serialize(s);
        }
    }
}
template <typename T>
inline void DeserializeObjectList(Serializer& s, std::list <T*> &olist)
{
    T* val;
    UINT size, i;
    s >> size;

    if (size) {
        for (i=0; i<size; i++) {
            val=new T();
            val->Deserialize(s);
            olist.push_back(val);
        }
    }
}
#endif
