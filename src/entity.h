/*
*entity.h
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
*This comprises our game objects.
*/

#ifndef ENTITY_H
#define ENTITY_H
#include <list>
#include <map>
#include <string>
#include "mud.h"
#include "conf.h"
#include "event.h"
#include "eventargs.h"
#include "variant.h"
#include "command.h"
#include "serialize.h"
#include "serializer.hpp"

class Entity:public ISerializable, public EventManager, public Commandable
{
    std::list <Entity*> _contents;
    std::list <Component*> *_components;
    std::map <std::string,Variant> *_vars;
#ifdef OLC
    std::vector <struct OLC_DATA*> *_olcs;
#endif
    VNUM _onum;
    Entity* _location;
    int _type;
    std::string _name;
    std::string _desc;
    std::string _script; //the scripting associated with this object.
public:
    Entity(void);
    virtual ~Entity(void);
    virtual std::string GetName(void) const;
    virtual void SetName(const std::string &s);
    virtual std::string GetDescription(void) const;
    virtual void SetDescription(const std::string &s);
    virtual std::string GetScript(void) const;
    virtual void SetScript(const std::string &script);
    /*
    *Returns the location of this object.
    *Return: A pointer to the entity in which this object is located, or NULL if the object does not have  a parent location.
    */
    Entity* GetLocation(void) const;
    /*
    *Sets the location of the object
    *Param: [in] The object to move the entity to.
    */
    void SetLocation(Entity* location);
    /*
    *Returns the type of the entity.
    *Return: 1 for object, 2 for room.
    *Note: don't add more types, this is just used for assigning the entity's objhect to the correct vnum table.
    */
    int GetType(void) const;
    /*
    *Sets the object type.
    *Param: [in] the type of the object (1 for object, 2 for room).
    */
    void SetType(int type);
    /*
    *Returns: The object's vnum.
    */
    VNUM GetOnum(void) const;
    /*
    *Sets the object's num.
    *Param: [in] the vnum for the object.
    */
    void SetOnum(VNUM num);
    virtual void Serialize(Serializer& ar);
    virtual void Deserialize(Serializer& ar);
    /*
    *Called when the player looks at the object.
    *Param: [in] a pointer to the player who looked at the object.
    *Returns: The Object description.
    */
    virtual std::string DoLook(Player* mobile);
    /*
    *Checks to see if the object can enter the object on which this function is being called on.
    *Return: True on success, false on failure.
    */
    virtual BOOL CanReceive(Entity* obj) const;
    /*
    *Moves this object to the target
    *Param: [in] the destination where the object needs to go.
    *Return: true on success, false on failure.
    */
    virtual BOOL MoveTo(Entity* targ);
    /*
    *Adds the specified component to the list.
    *Param: [in] a pointer to the component object.
    *Return: true if the component could be added, false otherwise.
    */
    BOOL AddComponent(Component* component);
    /*
    *Removes the specified component from the list.
    *Param: [in] a pointer to the component.
    *Return: true on success, false on failure.
    */
    BOOL RemoveComponent(Component* component);
    /*
    *Checks for the existance of a component by name.
    **Param: [in] the name of the component.
    *Return: True if the component exists, false otherwise.
    */
    bool HasComponent(const std::string &name);
    /*
    *Attaches the object to all the components in the list.
    *Should be called after adding a component.
    *Param: [in] the object to attach the components to.
    */
    void Attach(Entity * obj);
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
        (*_vars)[name]=val;
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
    BOOL GetPersistents(const std::string &name) const;
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
//Operators to retrieve variants.
    /*
    *[]
    *returns: a reference to the variant.
    *Param: [in] the name of the variant to retrieve.
    */
    Variant& operator [](const std::string & name) const;
    Variant& operator [](const char* name) const;

#ifdef OLC
//olc-based functions.
    /*
    *Adds an OLC entry to the list.
    *Params: [in] the name of the OLC item. (This is what shows in the menu).
    *Params: The prompt to show when asking for input.
    *[in] The type of OLC input function.
    *[in] The callback to use when this option is called.
    */
    void AddOlc(const std::string &name, const std::string &prompt, OLC_INPUT_TYPE type, FP_INPUT callback);
    /*
    *Used for removing an olc by name.
    *Param: [in] the name of the olc node to remove.
    *return: True on success, false if the node wasn't found.
    */
    BOOL RemoveOlc(const std::string &name);
    /*
    *Returns a copy of pointers to the OLC_DATA structures stored.
    *Param: [out] a list of OLC_DATA* where the pointers can be coppied.
    */
    void ListOlcs(std::vector<struct OLC_DATA*> &out);
    /*
    *Returns an olc structure by the specified index from the vector.
    *Param: [in] the index of the data in the vector to retrieve.
    *Return: a pointer to the data, NULL if index was invalid.
    */
    OLC_DATA* GetOlcByIndex(int index);
#endif

};

#endif
