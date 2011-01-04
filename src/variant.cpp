/*
*variant.cpp
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


#include "variant.h"
#include "exception.h"
#include "serializer.hpp"
#include "world.h"

Variant::Variant(void)
{
    type=VAR_EMPTY;
}
Variant::Variant(unsigned int s)
{
    u32=s;
    type=VAR_UINT;
}
Variant::Variant(int s)
{
    i32=s;
    type=VAR_INT;
}
Variant::Variant(unsigned short int s)
{
    u16=s;
    type=VAR_USHORT;
}
Variant::Variant(short int s)
{
    i16=s;
    type=VAR_SHORT;
}
Variant::Variant(unsigned char s)
{
    ubyte=s;
    type=VAR_UBYTE;
}
Variant::Variant(char s)
{
    byte=s;
    type=VAR_BYTE;
}
Variant::Variant(const std::string &s)
{
    str=s;
    type=VAR_STR;
}
Variant::Variant(const char* s)
{
    str=s;
    type=VAR_STR;
}
Variant::Variant(float s)
{
    f=s;
    type=VAR_FLOAT;
}
Variant::Variant(double s)
{
    d=s;
    type=VAR_DOUBLE;
}

void Variant::SetPersistents(BOOL s)
{
    _persistent = s;
}
BOOL Variant::GetPersistents(void) const
{
    return _persistent;
}

VARIABLE_TYPE Variant::Typeof(void) const
{
    return type;
}

unsigned int Variant::GetUint(void) const
{
    return u32;
}
int Variant::GetInt(void) const
{
    return i32;
}
unsigned short int Variant::GetUshort(void) const
{
    return u16;
}
short int Variant::GetShort(void) const
{
    return i16;
}
unsigned char Variant::GetUbyte(void) const
{
    return ubyte;
}
char Variant::GetByte(void) const
{
    return byte;
}
std::string Variant::GetStr(void) const
{
    return str;
}
float Variant::GetFloat(void) const
{
    return f;
}
double Variant::GetDouble(void) const
{
    return d;
}

void Variant::SetUint(unsigned int s)
{
    u32 = s;
    type = VAR_UINT;
}
void Variant::SetInt(int s)
{
    i32 = s;
    type = VAR_INT;
}
void Variant::SetUshort(unsigned short s)
{
    u16 = s;
    type = VAR_USHORT;
}
void Variant::SetShort(short s)
{
    i16 = s;
    type = VAR_SHORT;
}
void Variant::SetUbyte(unsigned char s)
{
    ubyte = s;
    type = VAR_UBYTE;
}
void Variant::SetByte(char s)
{
    byte = s;
    type = VAR_BYTE;
}
void Variant::SetFloat(float s)
{
    f = s;
    type = VAR_FLOAT;
}
void Variant::SetDouble(double s)
{
    d = s;
    type = VAR_DOUBLE;
}
void Variant::SetStr(const std::string &s)
{
    str = s;
    type = VAR_STR;
}
void Variant::SetStr(const char* s)
{
    str = s;
    type = VAR_STR;
}

BOOL Variant::IsEmpty(void) const
{
    return (type==VAR_EMPTY);
}
BOOL Variant::IsUint() const
{
    return (type==VAR_UINT);
}
BOOL Variant::IsInt(void) const
{
    return (type==VAR_INT);
}
BOOL Variant::IsUshort(void) const
{
    return (type==VAR_USHORT);
}
BOOL Variant::IsShort(void) const
{
    return (type==VAR_SHORT);
}
BOOL Variant::IsUbyte(void) const
{
    return (type==VAR_UBYTE);
}
BOOL Variant::IsByte(void) const
{
    return (type==VAR_STR);
}
BOOL Variant::IsFloat(void) const
{
    return (type==VAR_FLOAT);
}
BOOL Variant::IsDouble(void) const
{
    return (type==VAR_DOUBLE);
}
BOOL Variant::IsNumber(void) const
{
    switch (type) {
    case VAR_UINT:
    case VAR_INT:
    case VAR_USHORT:
    case VAR_SHORT:
    case VAR_FLOAT:
    case VAR_DOUBLE:
        return true;
    default:
        return false;
    }
}
BOOL Variant::IsChar(void) const
{
    switch (type) {
    case VAR_UBYTE:
    case VAR_BYTE:
        return true;
    default:
        return false;
    }
}
BOOL Variant::IsString(void) const
{
    switch (type) {
    case VAR_STR:
        return true;
    default:
        return false;
    }
}

BOOL Variant::Compare(const Variant &var) const
{
    VARIABLE_TYPE vtype;

    if (var.Typeof()!=Typeof()) {
        return false;
    }

    vtype=Typeof();

    switch (type) {
    case VAR_UINT:
        return (GetUint()==var.GetUint());
        break;
    case VAR_INT:
        return (GetInt()==var.GetInt());
        break;
    case VAR_USHORT:
        return (GetUshort()==var.GetUshort());
        break;
    case VAR_SHORT:
        return (GetShort()==var.GetShort());
        break;
    case VAR_UBYTE:
        return (GetUbyte()==var.GetUbyte());
        break;
    case VAR_BYTE:
        return (GetByte()==var.GetByte());
        break;
    case VAR_STR:
        return (GetStr()==var.GetStr());
        break;
    case VAR_FLOAT:
        return (GetFloat()==var.GetFloat());
        break;
    case VAR_DOUBLE:
        return (GetDouble()==var.GetDouble());
        break;
    case VAR_EMPTY:
        throw(VariableEmptyException("Tried to compare value of empty variable."));
        break;
    }
    return false;
}
BOOL Variant::operator ==(const Variant &var)
{
    return Compare(var);
}
BOOL Variant::operator !=(const Variant &var)
{
    return (Compare(var)==1?0:1);
}

Variant& Variant::operator =(UINT s)
{
    type = VAR_UINT;
    u32 = s;
    return (*this);
}
Variant& Variant::operator =(int s)
{
    type = VAR_INT;
    i32 = s;
    return (*this);
}
Variant& Variant::operator =(unsigned short s)
{
    type = VAR_USHORT;
    u16 = s;
    return (*this);
}
Variant& Variant::operator =(short s)
{
    type = VAR_SHORT;
    i16 = s;
    return (*this);
}
Variant& Variant::operator =(unsigned char s)
{
    type = VAR_UBYTE;
    ubyte = s;
    return (*this);
}
Variant& Variant::operator =(char s)
{
    type = VAR_BYTE;
    byte = s;
    return (*this);
}
Variant& Variant::operator =(float s)
{
    type = VAR_FLOAT;
    f = s;
    return (*this);
}
Variant& Variant::operator =(double s)
{
    type = VAR_DOUBLE;
    d = s;
    return (*this);
}
Variant& Variant::operator =(std::string s)
{
    type = VAR_STR;
    str = s;
    return (*this);
}

void Variant::Serialize(Serializer& ar)
{
    VARIABLE_TYPE t=Typeof();

    ar << t;

    switch (type) {
    case VAR_UINT:
        ar << u32;
        break;
    case VAR_INT:
        ar << i32;
        break;
    case VAR_USHORT:
        ar << u16;
        break;
    case VAR_SHORT:
        ar << i16;
        break;
    case VAR_UBYTE:
        ar << ubyte;
        break;
    case VAR_BYTE:
        ar << byte;
        break;
    case VAR_STR:
        ar << str;
        break;
    case VAR_FLOAT:
        ar << f;
        break;
    case VAR_DOUBLE:
        ar << d;
        break;
    case VAR_EMPTY:
        throw(VariableEmptyException("Tried to serialize value of empty variable."));
        break;
    }
}
void Variant::Deserialize(Serializer& ar)
{
    VARIABLE_TYPE t;
    ar >> t;
    type = t;

    switch (type) {
    case VAR_UINT:
        ar >> u32;
        break;
    case VAR_INT:
        ar >> i32;
        break;
    case VAR_USHORT:
        ar >> u16;
        break;
    case VAR_SHORT:
        ar >> i16;
        break;
    case VAR_UBYTE:
        ar >> ubyte;
        break;
    case VAR_BYTE:
        ar >> byte;
        break;
    case VAR_STR:
        ar >> str;
        break;
    case VAR_FLOAT:
        ar >> f;
        break;
    case VAR_DOUBLE:
        ar >> d;
        break;
//this should *never* happen
    case VAR_EMPTY:
        throw(VariableEmptyException("Tried to deserialize value of empty variable."));
        break;
    }
}

//math operator overloads

Variant& Variant::operator ++(void)
{
    switch(type) {
    default:
        throw(InvalidVariableTypeException("Tried to use \'++\' on a variable that isn't a number."));
        break;
    case VAR_UINT:
        u32++;
        break;
    case VAR_INT:
        i32++;
        break;
    case VAR_USHORT:
        u16++;
        break;
    case VAR_SHORT:
        i16++;
        break;
    case VAR_FLOAT:
        f++;
        break;
    case VAR_DOUBLE:
        d++;
        break;
    }
    return (*this);
}

Variant& Variant::operator --(void)
{
    switch(type) {
    default:
        throw(InvalidVariableTypeException("Tried to use \'--\' on a variable that isn't a number."));
        break;
    case VAR_UINT:
        u32--;
        break;
    case VAR_INT:
        i32--;
        break;
    case VAR_USHORT:
        u16--;
        break;
    case VAR_SHORT:
        i16--;
        break;
    case VAR_FLOAT:
        f--;
        break;
    case VAR_DOUBLE:
        d--;
        break;
    }
    return (*this);
}

Variant Variant::operator ++(int u)
{
    Variant var = (*this);
    ++(*this);
    return var;
}
Variant Variant::operator --(int u)
{
    Variant var = (*this);
    --(*this);
    return var;
}
