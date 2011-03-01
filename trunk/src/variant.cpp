#include <tinyxml.h>
#include <boost/lexical_cast.hpp>
#include "variant.h"
#include "exception.h"
#include "world.h"

Variant::Variant()
{
    type = VAR_EMPTY;
    i32 = 0;
}
Variant::Variant(const Variant& var)
{
    type = var.Typeof();
    switch(type) {
    case VAR_BYTE:
        byte = var.GetByte();
        break;
    case VAR_INT:
        i32=var.GetInt();
        break;
    case VAR_DOUBLE:
        d=var.GetDouble();
        break;
    case VAR_STR:
        str=var.GetStr();;
    case VAR_EMPTY:
    default:
        break;
    }
}
Variant::Variant(int s)
{
    i32 = s;
    type = VAR_INT;
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
Variant::Variant(double s)
{
    d=s;
    type=VAR_DOUBLE;
}

VARIABLE_TYPE Variant::Typeof() const
{
    return type;
}

int Variant::GetInt() const
{
    return i32;
}
char Variant::GetByte() const
{
    return byte;
}
std::string Variant::GetStr() const
{
    return str;
}
double Variant::GetDouble() const
{
    return d;
}

void Variant::SetInt(int s)
{
    i32 = s;
    type = VAR_INT;
}
void Variant::SetByte(char s)
{
    byte = s;
    type = VAR_BYTE;
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

BOOL Variant::IsEmpty() const
{
    return (type==VAR_EMPTY);
}
BOOL Variant::IsInt() const
{
    return (type==VAR_INT);
}
BOOL Variant::IsByte() const
{
    return (type==VAR_STR);
}
BOOL Variant::IsDouble() const
{
    return (type==VAR_DOUBLE);
}
BOOL Variant::IsNumber() const
{
    switch (type) {
    case VAR_INT:
    case VAR_DOUBLE:
        return true;
    default:
        return false;
    }
}
BOOL Variant::IsChar() const
{
    return (type == VAR_BYTE? true:false);
}
BOOL Variant::IsString() const
{
    return (type == VAR_STR);
}

BOOL Variant::Compare(const Variant &var) const
{
    if (var.Typeof()!=Typeof()) {
        return false;
    }

    switch (type) {
    case VAR_INT:
        return (GetInt()==var.GetInt());
        break;
    case VAR_BYTE:
        return (GetByte()==var.GetByte());
        break;
    case VAR_STR:
        return (GetStr()==var.GetStr());
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

Variant& Variant::operator =(int s)
{
    type = VAR_INT;
    i32 = s;
    return (*this);
}
Variant& Variant::operator =(char s)
{
    type = VAR_BYTE;
    byte = s;
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

void Variant::SetType(VARIABLE_TYPE t)
{
    type = t;
}

void Variant::Serialize(TiXmlElement* root)
{
    TiXmlElement* var = new TiXmlElement("variable");
    var->SetAttribute("type", (int)Typeof());

    switch (type) {
    case VAR_INT:
        var->SetAttribute("value", i32);
        break;
    case VAR_BYTE:
        var->SetAttribute("value", byte);
        break;
    case VAR_STR:
        var->SetAttribute("value", str.c_str());
        break;
    case VAR_DOUBLE:
        var->SetDoubleAttribute("value", d);
        break;
    case VAR_EMPTY:
        var->SetAttribute("value", 0);
        break;
    }
    root->LinkEndChild(var);
}
void Variant::Deserialize(TiXmlElement* var)
{
    int tmp;

    var->Attribute("type", &tmp);
    type = (VARIABLE_TYPE)tmp;

    switch (type) {
    case VAR_INT:
        var->Attribute("value", &i32);
        break;
    case VAR_BYTE:
        var->Attribute("value", &tmp);
        byte = (char)tmp;
        break;
    case VAR_STR:
        str = var->Attribute("value");
        break;
    case VAR_DOUBLE:
        var->Attribute("value", &d);
        break;
    case VAR_EMPTY:
        break;
    }
}

//math operator overloads

Variant& Variant::operator ++()
{
    switch(type) {
    default:
        throw(InvalidVariableTypeException("Tried to use \'++\' on a variable that isn't a number."));
        break;
    case VAR_INT:
        i32++;
        break;
    }
    return (*this);
}

Variant& Variant::operator --()
{
    switch(type) {
    default:
        throw(InvalidVariableTypeException("Tried to use \'--\' on a variable that isn't a number."));
        break;
    case VAR_INT:
        i32--;
        break;
    }
    return (*this);
}

Variant Variant::operator +(Variant var)
{
    VARIABLE_TYPE vtype = var.Typeof();

    switch(type) {
//string
    case VAR_STR:
        switch(vtype) {
        case VAR_STR:
            return Variant(str+var.GetStr());
        case VAR_BYTE:
            return Variant(str+var.GetByte());
        case VAR_INT:
            return Variant(str + boost::lexical_cast<std::string>(var.GetInt()));
        case VAR_DOUBLE:
            return Variant(str + boost::lexical_cast<std::string>(var.GetDouble()));
        default:
            throw(VariableEmptyException("Tried to add an empty variant to another variant."));
        }
//integer
    case VAR_INT:
        switch(vtype) {
        case VAR_INT:
            return Variant(i32 + var.GetInt());
        case VAR_DOUBLE:
            return Variant(i32 + var.GetDouble());
        case VAR_BYTE:
            return Variant(i32 + var.GetByte());
        case VAR_STR:
            throw(InvalidVariableTypeException("Tried to add a string to an integer."));
        default:
            throw(VariableEmptyException("Tried to add an empty variant to another variant."));
        }
//double
    case VAR_DOUBLE:
        switch(vtype) {
        case VAR_DOUBLE:
            return Variant(d + var.GetDouble());
        case VAR_INT:
            return Variant(d + var.GetInt());
        case VAR_BYTE:
            throw(InvalidVariableTypeException("Tried to add byte to double."));
        case VAR_STR:
            throw(InvalidVariableTypeException("Tried to add a string to a double."));
        default:
            throw(VariableEmptyException("Tried to add an empty variant to another variant."));
        }
//byte
    case VAR_BYTE:
        switch(vtype) {
        case VAR_INT:
            return Variant(byte + var.GetInt());
        case VAR_BYTE:
            return Variant(byte + var.GetByte());
        default:
            throw(InvalidVariableTypeException("Tried to add byte to an illegal value."));
        }
    default:
        throw(InvalidVariableTypeException("Tried to add empty variable."));
    }
}
Variant& Variant::operator +=(Variant var)
{
    (*this)+var;
    return (*this);
}
