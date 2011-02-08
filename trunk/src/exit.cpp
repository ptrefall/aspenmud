#include <string>
#include <tinyxml.h>
#include "exit.h"
#include "living.h"

Exit::Exit(VNUM from, VNUM to)
{
    _to=to;
    _from=from;
}
Exit::Exit(void)
{
    _from=EXIT_NOWHERE;
    _to=EXIT_NOWHERE;
}
Exit::~Exit(void)
{
}

VNUM Exit::GetFrom(void) const
{
    return _from;
}
void Exit::SetFrom(VNUM from)
{
    _from=from;
}

VNUM Exit::GetTo(void) const
{
    return _to;
}
void Exit::SetTo(VNUM to)
{
    _to=to;
}

std::string Exit::GetName(void) const
{
    return _name;
}
void Exit::SetName(const std::string &name)
{
    _name=name;
}

std::string Exit::GetAlias(void) const
{
    return _alias;
}
void Exit::SetAlias(const std::string &alias)
{
    _alias=alias;
}

BOOL Exit::CanEnter(Living* mobile)
{
    return true;
}

void Exit::Serialize(TiXmlElement* root)
{
    TiXmlElement* node = new TiXmlElement("exit");
    node->SetAttribute("name", _name.c_str());
    node->SetAttribute("alias", _alias.c_str());
    node->SetAttribute("to", _to);
    node->SetAttribute("from", _from);
    root->LinkEndChild(root);
}
void Exit::Deserialize(TiXmlElement* node)
{
    node->Attribute("to", &_to);
    node->Attribute("from", &_from);
    _name = node->Attribute("name");
    _alias = node->Attribute("alias");
}
