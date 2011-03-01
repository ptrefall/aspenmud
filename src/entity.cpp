#include <string>
#include <list>
#include <map>
#include <tinyxml.h>
#include "entity.h"
#include "world.h"
#include "room.h"
#include "eventargs.h"
#include "utils.h"
#include "command.h"
#include "olc.h"
#include "editor.h"

#ifdef OLC
OLC_INPUT(olc_entity_name);
OLC_INPUT(olc_entity_description);
#endif

Entity::Entity(void)
{
    _name="A blank object";
    _desc="You see nothing special.";
    _script="--scripting starts here\n";
    _location=NULL;
    _type=1;
    _onum=0;
    _components=new std::list <Component*>();
#ifdef OLC
    _olcs = new std::vector<struct OLC_DATA*>();
    AddOlc("name", "Please enter the name of the object", STRING, olc_entity_name);
    AddOlc("description", "Editing object description", EDITOR, olc_entity_description);
#endif

    events.RegisterEvent("PostLook", new Event());
    events.RegisterEvent("PreLook",new Event());
}
Entity::~Entity(void)
{
    std::list <Component*>::iterator it, itEnd;
    itEnd = _components->end();
    for (it=_components->begin(); it != itEnd; ++it) {
        delete (*it);
    }
    delete _components;

#ifdef OLC
    std::vector<struct OLC_DATA*>::iterator oit, oitEnd;
    oitEnd = _olcs->end();
    for (oit=_olcs->begin(); oit != oitEnd; ++oit) {
        delete (*oit);
    }
    delete _olcs;
#endif
}

std::string Entity::GetName(void) const
{
    return _name;
}
void Entity::SetName(const std::string &s)
{
    _name=s;
}

std::string Entity::GetDescription(void) const
{
    return _desc;
}
void Entity::SetDescription(const std::string &s)
{
    _desc=s;
}

std::string Entity::GetScript(void) const
{
    return _script;
}
void Entity::SetScript(const std::string &script)
{
    _script=script;
}

Entity* Entity::GetLocation(void) const
{
    return _location;
}
void Entity::SetLocation(Entity* location)
{
    _location=location;
}

int Entity::GetType(void) const
{
    return _type;
}
void Entity::SetType(int type)
{
    _type=type;
}

VNUM Entity::GetOnum(void) const
{
    return _onum;
}
void Entity::SetOnum(VNUM num)
{
    _onum=num;
}

void Entity::Serialize(TiXmlElement* root)
{
    TiXmlElement* ent = new TiXmlElement("entity");
    TiXmlElement *components = new TiXmlElement("components");
    TiXmlElement* component = NULL;
    TiXmlElement* contents = new TiXmlElement("contents");
    TiXmlElement* properties = new TiXmlElement("properties");
    std::list <Component*>::iterator it;
    std::list<Component*>::iterator itEnd = _components->end();
    std::list<Entity*>::iterator cit;
    std::list<Entity*>::iterator citEnd=_contents.end();

    if (_contents.size()) {
        for (cit = _contents.begin(); cit != citEnd; ++cit) {
            (*cit)->Serialize(ent);
        }
    }
    ent->LinkEndChild(contents);

    if (_components->size()) {
        for (it=_components->begin(); it != itEnd; ++it) {
            component = new TiXmlElement("component");
            component->SetAttribute("name", (*it)->GetName().c_str());
            components->LinkEndChild(component);
        }
    }
    ent->LinkEndChild(components);

    variables.Serialize(properties);
    ent->LinkEndChild(properties);

    ent->SetAttribute("name", _name.c_str());
    ent->SetAttribute("desc", _desc.c_str());
    ent->SetAttribute("script", _script.c_str());
    ent->SetAttribute("onum", _onum);
    ent->SetAttribute("type", _type);
    ent->SetAttribute("location", (_location?_location->GetOnum():0));

    root->LinkEndChild(ent);
}
void Entity::Deserialize(TiXmlElement* root)
{
    TiXmlElement* components = NULL;
    TiXmlElement* component = NULL;
    TiXmlElement* obj = NULL;
    TiXmlElement* contents = NULL;
    TiXmlElement* properties = NULL;
    TiXmlNode* node = NULL;
    int loc;

//contents
    node = root->FirstChild("contents");
    if (node) {
        contents = node->ToElement();
        for (node = contents->FirstChild(); node; node = node->NextSibling()) {
            obj = node->ToElement();
            Entity* object = new Entity();
            object->Deserialize(obj);
            object->SetLocation(this);
            _contents.push_back(object);
        }
    }

    node = root->FirstChild("components");
    if (node) {
        components = node->ToElement();
        for (node = components->FirstChild(); node; node = node->NextSibling()) {
            component = node->ToElement();
            AddComponent(world->CreateComponent(component->Attribute("name")));
        }
    }

    node = root->FirstChild("properties");
    if (node) {
        properties = node->ToElement();
        variables.Deserialize(properties);
    }

    _name = root->Attribute("name");
    _desc = root->Attribute("desc");
    _script = root->Attribute("script");
    root->Attribute("onum", &_onum);
    root->Attribute("type", &_type);
    root->Attribute("location", &loc);
    if (!loc) {
        _location=NULL;
    } else {
        if (_type==1) {
//it's an object
            _location=world->GetObject(loc);
        } else if (_type==2) {
//it's a room
            _location=world->GetRoom(loc);
        }
    }
}

std::string Entity::DoLook(Player* mobile)
{
    std::string str;

    LookArgs* args=new LookArgs(mobile,this,str);
    events.CallEvent("PreLook", args, (void*)mobile);
    str+=Capitalize(this->GetName())+"\n";
    str+=this->GetDescription()+"\n";
    events.CallEvent("PostLook", args, (void*)mobile);
    delete args;
    return str;
}

BOOL Entity::CanReceive(Entity* obj) const
{
    return true;
}

BOOL Entity::MoveTo(Entity* targ)
{
    if (targ->CanReceive(this)) {
        _location=targ;
        return true;
    }
    return false;
}

BOOL Entity::AddComponent(Component* component)
{
    if (component==NULL) {
        return false;
    }

    if (HasComponent(component->GetName())) {
        return false;
    }
    _components->push_back(component);
    component->Attach(this);
    return true;
}
BOOL Entity::RemoveComponent(Component* component)
{
    if (!HasComponent(component->GetName())) {
        return false;
    }

    _components->remove(component);
    component->Detach();
    return true;
}
bool Entity::HasComponent(const std::string &name)
{
    std::list <Component*>::iterator it, itEnd;

    itEnd = _components->end();
    for (it = _components->begin(); it != itEnd; ++it) {
        if ((*it)->GetName()==name) {
            return true;
        }
    }

    return false;
}

void Entity::Attach(Entity* obj)
{
    std::list <Component*>::iterator it, itEnd;

    itEnd = _components->end();
    for (it = _components->begin(); it != itEnd; ++it) {
        (*it)->Attach(obj);
    }
}

#ifdef OLC
void Entity::AddOlc(const std::string &name, const std::string &prompt, OLC_INPUT_TYPE type, FP_INPUT callback)
{
    struct OLC_DATA* data = NULL;
    if ((name == "") || (callback == NULL)) {
        return;
    }

    data = new OLC_DATA();
    data->name = name;
    data->prompt = prompt;
    data-> type = type;
    data->callback = callback;
    _olcs->push_back(data);
}
BOOL Entity::RemoveOlc(const std::string &name)
{
    std::vector<struct OLC_DATA*>::iterator it, itEnd;

    itEnd = _olcs->end();
    for (it = _olcs->begin(); it != itEnd; ++it) {
        if ((*it)->name == name) {
            _olcs->erase(it);
            return true;
        }
    }

    return false;
}
void Entity::ListOlcs(std::vector<struct OLC_DATA*> &out)
{
    std::vector<struct OLC_DATA*>::iterator it;
    std::vector<struct OLC_DATA*>::iterator end = _olcs->end();

    for (it = _olcs->begin(); it != end; it++) {
        out.push_back((*it));
    }
}
OLC_DATA* Entity::GetOlcByIndex(int index)
{
    index--;

    if (((int)_olcs->size() < index+1) || (index < 0 )) {
        return NULL;
    }

    return (_olcs->at(index));
}

OLC_INPUT(olc_entity_name)
{
    if (input->Typeof() != VAR_STR) {
        mob->Message(MSG_ERROR, "Invalid input.");
        return;
    }

//we have a string, lets set that to the name.
    ed->SetName(input->GetStr());
}

OLC_INPUT(olc_entity_description)
{
    Editor* edit = new Editor();
    edit->SetArg(ed);
    edit->events.AddCallback("load", entity_description_editor_load);
    edit->events.AddCallback("save", entity_description_editor_save);
    edit->EnterEditor(mob);
}

EVENT(entity_description_editor_load)
{
    OneArg* arg = (OneArg*)args;
    Editor* ed = (Editor*)arg->_arg;
    Entity* obj = (Entity*)ed->GetArg();
    std::vector<std::string> lines;
    std::vector<std::string>::iterator it;
    std::vector<std::string>::iterator itEnd;
    Tokenize(obj->GetDescription(), lines, "\n");

    itEnd = lines.end();
    for (it = lines.begin(); it != itEnd; ++it) {
        ed->Add((*it));
    }
}
EVENT(entity_description_editor_save)
{
    OneArg* arg = (OneArg*)args;
    Editor* ed = (Editor*)arg->_arg;
    Entity* obj = (Entity*)ed->GetArg();
    std::string desc;
    std::vector<std::string>::iterator it;
    std::vector<std::string>::iterator itEnd;

    std::vector<std::string>* lines = ed->GetLines();
    itEnd = lines->end();
    for (it = lines->begin(); it != itEnd; ++it) {
        desc += "\n" + (*it);
    }
    obj->SetDescription(desc);
}
#endif
