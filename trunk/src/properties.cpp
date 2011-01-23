#include "properties.h"
#include "entity.h"
#include "exception.h"
#include "variant.h"
#include <map>
#include <string>
#include <vector>

struct PropertySerializationNode {
    std::string name;
    int type;
    void* var;
};

PropertyMap::PropertyMap()
{
}
PropertyMap::~PropertyMap()
{
}

Variant& PropertyMap::operator [](const std::string &name)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return _properties[name];
}
Variant& PropertyMap::operator [](const char* name)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return _properties[name];
}

BOOL PropertyMap::RemoveVar(const std::string &name)
{
    if (VarExists(name)) {
        _properties.erase(name);
        return true;
    }
    return false;
}
BOOL PropertyMap::VarExists(const std::string &name) const
{
    return (_properties.count(name));
}
void PropertyMap::SetPersistents(const std::string &name, BOOL s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    _properties[name].SetPersistents(s);
}
BOOL PropertyMap::GetPersistents(const std::string &name)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return _properties[name].GetPersistents();
}

int PropertyMap::GetInt(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return ((*_properties.find(name)).second).GetInt();
}
UINT PropertyMap::GetUint(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return ((*_properties.find(name)).second).GetUint();
}
unsigned short PropertyMap::GetUshort(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return ((*_properties.find(name)).second).GetUshort();
}
short PropertyMap::GetShort(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return ((*_properties.find(name)).second).GetShort();
}
char PropertyMap::GetByte(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return ((*_properties.find(name)).second).GetByte();
}
unsigned char PropertyMap::GetUbyte(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return ((*_properties.find(name)).second).GetUbyte();
}
float PropertyMap::GetFloat(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return ((*_properties.find(name)).second).GetFloat();
}
std::string PropertyMap::GetString(const std::string &name) const
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    return ((*_properties.find(name)).second).GetStr();
}
void PropertyMap::SetInt(const std::string &name, int s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    _properties[name].SetInt(s);
}
void PropertyMap::SetUint(const std::string &name, unsigned int s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    _properties[name].SetUint(s);
}
void PropertyMap::SetUshort(const std::string &name, unsigned short s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    _properties[name].SetUshort(s);
}
void PropertyMap::SetShort(const std::string &name, short s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    _properties[name].SetShort(s);
}
void PropertyMap::SetUbyte(const std::string &name, unsigned char s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    _properties[name].SetUbyte(s);
}
void PropertyMap::SetByte(const std::string &name, char s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    _properties[name].SetByte(s);
}
void PropertyMap::SetFloat(const std::string &name, float s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    _properties[name].SetFloat(s);
}
void PropertyMap::SetDouble(const std::string &name, double s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    _properties[name].SetDouble(s);
}
void PropertyMap::SetStr(const std::string &name, const std::string &s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    _properties[name].SetStr(s);
}
void PropertyMap::SetStr(const std::string &name, const char* s)
{
    if (!VarExists(name)) {
        throw(VarNotFoundException(std::string("Variable ")+name+std::string(" doesn't exist. ")+WHERE()));
    }

    _properties[name].SetStr(s);
}
