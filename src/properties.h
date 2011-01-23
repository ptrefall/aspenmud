/*
*a module for adding both property maps (<string, variant> and property lists to an object.
*/

#ifndef PROPERTY_H
#define PROPERTY_H
#include "mud.h"
#include "conf.h"
#include "variant.h"
#include <string>
#include <map>
#include <vector>

class PropertyMap
{
protected:
    std::map<std::string, Variant> _properties;
    std::map<std::string, std::vector<std::map<std::string, Variant> > > _propertyList;
public:
    PropertyMap();
    ~PropertyMap();

    Variant& operator [](const std::string &key);
    Variant& operator [](const char* name);
    /*
    *Adds a variable to the container
    *Param: [in] the name of the variable.
    *[in] The value of the variable.
    *Return: True if the variable was added, false otherwise.
    */
    template <class T>
    BOOL AddVar(const std::string name, T val) {
        if (VarExists(name)) {
            return false;
        }
        _properties[name]=val;
        return true;
    }
    /*
    *Removes a variable from the container.
    *Param: [in] The name of the variable.
    *Return: True if the variable was removed, false otherwise.
    */
    BOOL RemoveVar(const std::string &name);
    /*
    *Checks to see if the specified variable exists
    *Param: [in] the name of the variable.
    *Return: true if the variable exists, false otherwise.
    */
    BOOL VarExists(const std::string &name) const;
    /*
    *Sets the persistents of a variable.
    *Param: [in] the name of the variable.
    *[in] True if the variable should be persistent (default), false otherwise.
    */
    void SetPersistents(const std::string &name, BOOL s);
    /*
     *Returns the persistents state for a variable.
     *Return: True if the variable is persistent, false otherwise.
     */
    BOOL GetPersistents(const std::string &name);
    /*
    *Getters
    *Param: [in] the name of the variable to get
    *Return: The value of the variable.
    *Throws: VarNotFoundException if the requested variable doesn't exist.
    */
    int GetInt(const std::string &name) const;
    UINT GetUint(const std::string &name) const;
    unsigned short GetUshort(const std::string &name) const;
    short GetShort(const std::string &name) const;
    char GetByte(const std::string &name) const;
    unsigned char GetUbyte(const std::string &name) const;
    float GetFloat(const std::string &name) const;
    double GetDouble(const std::string &name) const;
    std::string GetString(const std::string &name) const;
    /*
    *Setters
    *Sets the value at the specified variable.
    *Param: [in] the name of the variable.
    *[in] the value of the variable.
    *Throws: VarNotFound exception, if the variable specified doesn't exist.
    */
    void SetInt(const std::string &name, int s);
    void SetUint(const std::string &name, unsigned int s);
    void SetUshort(const std::string &name, unsigned short s);
    void SetShort(const std::string &name, short s);
    void SetByte(const std::string &name, char s);
    void SetUbyte(const std::string &name, unsigned char s);
    void SetFloat(const std::string &name, float s);
    void SetDouble(const std::string &name, double s);
    void SetStr(const std::string &name, const std::string &s);
    void SetStr(const std::string &name, const char* s);
};
#endif
