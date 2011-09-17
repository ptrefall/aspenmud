#include <tinyxml.h>
#include <string>
#include <map>
#include "socials.h"
#include "world.h"
#include "living.h"
#include "command.h"
#include "utils.h"

Socials* Socials::_ptr;
Socials* Socials::GetPtr(void)
{
  if (!Socials::_ptr)
    {
      Socials::_ptr = new Socials();
      Socials::_ptr->Initialize();
    }
  return Socials::_ptr;
}

void Socials::Initialize(void)
{
  _socid = 1;
  _slist = new std::map <std::string, SOCIAL_DATA*>();
}
Socials::~Socials(void)
{
  std::map<std::string, SOCIAL_DATA*>::iterator it, itEnd;

  if (_slist&&_slist->size())
    {
      itEnd = _slist->end();
      for (it = _slist->begin(); it != itEnd; ++it)
        {
          delete (*it).second;
        }
    }
  if (_slist)
    {
      delete _slist;
    }
}

void Socials::Save(void)
{
  TiXmlDocument doc;
  TiXmlElement* socials = new TiXmlElement("socials");
  TiXmlElement* social = NULL;
  TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "", "");
  doc.LinkEndChild(decl);

  std::map<std::string, SOCIAL_DATA*>::iterator it, itEnd;
  SOCIAL_DATA* d = NULL;

//if there are socials, print them.
  if (_slist->size())
    {
      itEnd = _slist->end();
      for (it = _slist->begin(); it != itEnd; ++it)
        {
          d = (*it).second;
          social = new TiXmlElement("social");
          social->SetAttribute("id", (int)d->id);
          social->SetAttribute("name", d->name.c_str());
          social->SetAttribute("ynotarg", d->ynotarg.c_str());
          social->SetAttribute("rnotarg", d->rnotarg.c_str());
          social->SetAttribute("ttarg", d->ttarg.c_str());
          social->SetAttribute("rtarg", d->rtarg.c_str());
          social->SetAttribute("ytarg", d->ytarg.c_str());
          socials->LinkEndChild(social);
        }
    }
  doc.LinkEndChild(socials);
  doc.SaveFile(SOCIALS_FILE);
}
void Socials::Load(void)
{
  World* world = World::GetPtr();

  int id;
  TiXmlDocument doc(SOCIALS_FILE);
  TiXmlElement* socials = NULL;
  TiXmlElement* social = NULL;
  TiXmlNode* node = NULL;
  SOCIAL_DATA* data = NULL;

  if (!FileExists(SOCIALS_FILE))
    {
#ifndef NO_INIT_DEFAULTS
      InitializeDefaultSocials();
#else
      world->WriteLog("No socials file exists, and NO_INIT_DEFAULTS was enabled.", CRIT);
#endif
      return;
    }
  if (!doc.LoadFile())
    {
      world->WriteLog("Could not load XML socials file.");
      return;
    }

  node = doc.FirstChild("socials");
  if (node)
    {
      socials = node->ToElement();

      for (node = socials->FirstChild(); node; node = node->NextSibling())
        {
          social = node->ToElement();
          data = new SOCIAL_DATA();
          social->Attribute("id", &id);
          data->id = id;
          data->name = social->Attribute("name");
          data->ynotarg = social->Attribute("ynotarg");
          data->rnotarg = social->Attribute("rnotarg");
          data->ttarg = social->Attribute("ttarg");
          data->rtarg = social->Attribute("rtarg");
          data->ytarg = social->Attribute("ytarg");

          if ((unsigned int)id > _socid)
            {
              _socid = id+1;
            }

          (*_slist)[data->name] = data;
          data = NULL;
        }
    }
}

BOOL Socials::SocialExists(const std::string &name) const
{
  return (_slist->count(name) == 0? false:true);
}
BOOL Socials::AddSocial(const std::string &name, const std::string &ynotarg, const std::string &rnotarg, const std::string &ytarg, const std::string &rtarg, const std::string &ttarg)
{
  if (SocialExists(name))
    {
      return false;
    }

  SOCIAL_DATA* data = new SOCIAL_DATA();
  data->name = name;
  data->ynotarg = ynotarg;
  data->rnotarg = rnotarg;
  data->ttarg = ttarg;
  data->rtarg = rtarg;
  data->ytarg = ytarg;
  data->id = _socid;
  _socid++;
  (*_slist)[name] = data;
  return true;
}
SOCIAL_DATA* Socials::GetSocial(const std::string &name) const
{
  if (!SocialExists(name))
    {
      return NULL;
    }

  return (*_slist)[name];
}

void Socials::InitializeDefaultSocials(void)
{
#ifndef NO_INIT_DEFAULTS
  AddSocial("smile", "You %a %v.", "%N %a %vs.", "You %a %v at %T.", "%N %a %vs at %T.", "%N %a %vs at you.");
  AddSocial("grin", "You %v %a.", "%N %vs %a.", "You %v %a at %T.", "%N %vs %a at %T.", "%N %vs %a at you.");
  AddSocial("nod", "You %v %a.", "%N %vs %a.", "You %v %a at %T.", "%N %vs %a at %T.", "%N %vs %a at you.");
  AddSocial("frown", "You %v %a.", "%N %vs %a.", "You %v %a at %T.", "%N %vs %a at %T.", "%N %vs %a at you.");
  AddSocial("wave", "You %a %v.", "%N %a %vs.", "You %a %v to %T.", "%N %a %vs to %T.", "%N %a %vs to you.");
  AddSocial("agree", "You %a %v.", "%N %a %vs.", "You %a %v with %T.", "%N %a %vs with %T.", "%N %a %vs with you.");
  AddSocial("chuckle", "You %v %a.", "%N %vs %a.", "You %v %a at %T.", "%N %vs %a at %T.", "%N %vs %a at you.");
  AddSocial("cower", "You %v in a corner.", "%N %vs in a corner.", "You %v in fear from %T.", "%N %vs in fear from %T.", "%N %vs in fear from you.");
  AddSocial("cry", "You %v.", "%N %vs.", "You %v on %T's shoulder.", "%N %vs on %T's shoulder.", "%N %vs on your shoulder.");
  AddSocial("wink", "you %v suggestively", "%N %vs suggestively", "You %v suggestively at %T.", "%N %vs suggestively at %T.", "%N %vs suggestively at you.");
  AddSocial("addict", "You stand and proclaim, \"I am %N, and I am a mud addict.\"", "%N stands and proclaims, \"I am %N, and I am a mud addict.\"", "You stand and proclaim to %T, \"I am %N, and I am a mud addict.\"", "%N stands and proclaims to %T, \"I am %N, and I am a mud addict.\"", "%N proclaims to you, \"I am %N, and I am a mud addict.\"");
  AddSocial("aargh", "You shout, %v!", "%N shouts, %v!", "You shout, %v at %T.", "%N shouts, %v at %T.", "%N shouts, %v at you.");
  Save();
#endif
}

void Socials::AddCommands(void)
{
  std::map <std::string, SOCIAL_DATA*>::iterator it;
  std::map <std::string, SOCIAL_DATA*>::iterator itEnd;
  World* world = World::GetPtr();

  world->WriteLog("Adding social commands.");
  itEnd = _slist->end();
  for (it = _slist->begin(); it != itEnd; ++it)
    {
      CMDSocials*com = new CMDSocials();
      com->SetName((*it).second->name);
      com->SetType(social);
      com->SetSubcmd((*it).second->id);
      world->commands.AddCommand(com);
    }
}

EVENT(socials_shutdown)
{
  World* world = World::GetPtr();
  world->WriteLog("Cleaning up socials.");
  Socials* soc = Socials::GetPtr();
  soc->Save();
}

CMDSocials::CMDSocials()
{
}
BOOL CMDSocials::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
  Socials* soc = Socials::GetPtr();
  SOCIAL_DATA* data = soc->GetSocial(verb);
  if (data == NULL)
    {
      mobile->Message(MSG_ERROR, "That social doesn't exist.");
      return false;
    }
  return true;
}

BOOL InitializeSocials(void)
{
  World* world = World::GetPtr();
  world->WriteLog("Initializing socials.");
  Socials* soc = Socials::GetPtr();
  soc->Load();
  soc->AddCommands();
  world->events.AddCallback("Shutdown", socials_shutdown);
  world->events.AddCallback("Copyover", socials_shutdown);
  return true;
}
