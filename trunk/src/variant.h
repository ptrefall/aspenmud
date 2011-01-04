/*
*variant.h
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


/*
*This class is used to hold variables.
*It allows the user to specify the type for the variable.
*Also allows for serialization and deserialization, so they can be stored.
*/

#ifndef VARIANT_H
#define VARIANT_H
#include "mud.h"
#include "conf.h"
#include "exception.h"
#include "serializer.hpp"

//exceptions
/*
*Used for operations on an empty variable.
*/
class VariableEmptyException:public Exception
{
public:
    VariableEmptyException(const std::string &msg):Exception(msg) { }
};
/*
*Used when operations are performed on an invalid variable type.
*For example: var++, when var is a string.
*/
class InvalidVariableTypeException:public Exception
{
public:
    InvalidVariableTypeException(const std::string &msg):Exception(msg) { }
};

//variable types
enum VARIABLE_TYPE { VAR_EMPTY, VAR_UINT, VAR_INT, VAR_USHORT, VAR_SHORT, VAR_UBYTE, VAR_BYTE, VAR_FLOAT, VAR_DOUBLE, VAR_STR };

class Variant
{
    union {
        unsigned int u32;
        int i32;
        unsigned short int u16;
        short int i16;
        char byte;
        unsigned char ubyte;
        float f;
        double d;
    };
    std::string str;
    VARIABLE_TYPE type;
    BOOL _persistent;
public:
    Variant(void);
    Variant(unsigned int s);
    Variant(int s);
    Variant(unsigned short int s);
    Variant(short int s);
    Variant(unsigned char s);
    Variant(char s);
    Variant(const std::string &s);
    Variant(const char* s);
    Variant(float s);
    Variant(double s);

    /*
    *Sets the persistent state
    *This is used for variants that shouldn't be serialized when the serialization function is called.
    *Param: [in] True for the variant to remain persistent (default), false otherwise.
    */
    void SetPersistents(BOOL s);
    /*
    *Returns the persistent state of the variant.
    */
    BOOL GetPersistents(void) const;

    /*
    *Returns the type of the variable.
    */
    VARIABLE_TYPE Typeof(void) const;

//getters
    unsigned int GetUint(void) const;
    int GetInt(void) const;
    unsigned short GetUshort(void) const;
    short GetShort(void) const;
    unsigned char GetUbyte(void) const;
    char GetByte(void) const;
    std::string GetStr(void) const;
    float GetFloat(void) const;
    double GetDouble(void) const;
//setters
    void SetUint(unsigned int s);
    void SetInt(int s);
    void SetUshort(unsigned short s);
    void SetShort(short s);
    void SetUbyte(unsigned char s);
    void SetByte(char s);
    void SetFloat(float s);
    void SetDouble(double s);
    void SetStr(const std::string &s);
    void SetStr(const char* s);
//type checks
    BOOL IsEmpty(void) const;
    BOOL IsUint() const;
    BOOL IsInt(void) const;
    BOOL IsUshort(void) const;
    BOOL IsShort(void) const;
    BOOL IsUbyte(void) const;
    BOOL IsByte(void) const;
    BOOL IsFloat(void) const;
    BOOL IsDouble(void) const;
    BOOL IsNumber(void) const;
    BOOL IsChar(void) const;
    BOOL IsString(void) const;

    template <class T>
    T Get(void) const {
        switch (type) {
        case VAR_UINT:
            return u32;
            break;
        case VAR_INT:
            return i32;
            break;
        case VAR_USHORT:
            return u16;
            break;
        case VAR_SHORT:
            return i16;
            break;
        case VAR_UBYTE:
            return ubyte;
            break;
        case VAR_BYTE:
            return byte;
            break;
        case VAR_STR:
            return str;
            break;
        case VAR_FLOAT:
            return f;
            break;
        case VAR_DOUBLE:
            return d;
            break;
        case VAR_EMPTY:
            throw(Exception("Tried to get value of empty variable."));
            break;
        default:
            throw(Exception("Unknown variable type."));
            break;
        }
    }

    BOOL Compare(const Variant &var) const;

//operator overloads
    BOOL operator ==(const Variant &var);
    BOOL operator !=(const Variant &var);
//assignment overloads
    Variant& operator =(UINT s);
    Variant& operator =(int s);
    Variant& operator =(unsigned short s);
    Variant& operator =(short s);
    Variant& operator =(unsigned char s);
    Variant& operator =(char s);
    Variant& operator =(float s);
    Variant& operator =(double s);
    Variant& operator =(std::string s);
    template <class T>
    T operator ()(void)const {
        switch(type) {
        case VAR_UINT:
            return (GetUint());
            break;
        case VAR_INT:
            return (GetInt());
            break;
        case VAR_USHORT:
            return (GetUshort());
            break;
        case VAR_SHORT:
            return (GetShort());
            break;
        case VAR_UBYTE:
            return (GetUbyte());
            break;
        case VAR_BYTE:
            return (GetByte());
            break;
        case VAR_STR:
            return (GetStr());
            break;
        case VAR_FLOAT:
            return (GetFloat());
            break;
        case VAR_DOUBLE:
            return (GetDouble());
            break;
        case VAR_EMPTY:
        default:
            throw(Exception("Tried to get invalid variable type."));
            break;
        }
    }

//serialization
    void Serialize(Serializer& ar);
    void Deserialize(Serializer& ar);
//math operator overloads:
//prefix ++ and --
    Variant& operator ++(void);
    Variant& operator --(void);
//postfix ++ and --
    Variant operator ++(int);
    Variant operator --(int);

};
#endif