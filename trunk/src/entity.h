/*
*This comprises our game objects.
*/
#ifndef ENTITY_H
#define ENTITY_H
#include <tinyxml.h>
#include <list>
#include <map>
#include <string>
#include "mud.h"
#include "conf.h"
#include "event.h"
#include "eventargs.h"
#include "variant.h"
#include "command.h"
#include "property.hpp"
//events
EVENT(entity_description_editor_load);
EVENT(entity_description_editor_save);
class Entity
{
    std::list <Entity*> _contents;
    std::vector <Component*> *_components;
    std::vector<std::string>* _aliases;
#ifdef OLC
    std::vector <struct OLC_DATA*> *_olcs;
#endif
    VNUM _onum;
    Entity* _location;
    int _type;
    std::string _name;
    std::string _desc;
    std::string _script; //the scripting associated with this object.
    std::string _plural;
public:
//these are objects we need to store on each entity.
    EventManager events;
    Property variables;
    Commandable commands;

    Entity(void);
    virtual ~Entity(void);
    virtual std::string GetName(void) const;
    virtual void SetName(const std::string &s);
    virtual std::string GetDescription(void) const;
    virtual void SetDescription(const std::string &s);
    virtual std::string GetPlural();
    virtual void SetPlural(const std::string &s);
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
    virtual void Serialize(TiXmlElement* root);
    virtual void Deserialize(TiXmlElement* root);
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
    *Adds the specified alias.
    *Param: [in] the name of the alias to add.
    *Return: True if the alias could be added, false otherwise.
    */
    BOOL AddAlias(const std::string &alias);
    /*
    *checks to see if the alias exists.
    *Param: [in] the name of the alias.
    *Return: True on success, false on failure.
    */
    BOOL AliasExists(const std::string &name);
    /*
    *Returns a pointer to the list of aliases.
    *Return: a pointer to a vector of strings.
    */
    std::vector<std::string>* GetAliases();
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
