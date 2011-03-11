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
  std::map<std::string, SOCIAL_DATA*>::iterator it;
  std::map<std::string, SOCIAL_DATA*>::iterator itEnd;

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
  std::map<std::string, SOCIAL_DATA*>::iterator it;
  std::map<std::string, SOCIAL_DATA*>::iterator itEnd;
  FILE* output = fopen(SOCIALS_FILE, "w");
  SOCIAL_DATA* d = NULL;

  if (!output)
    {
      world->WriteLog("Could not save to socials file.");
      return;
    }
//print the number of socials.
  fprintf(output, "%u\n", (UINT)_slist->size());
//if there are socials, print them.
  if (_slist&&_slist->size())
    {
      itEnd = _slist->end();
      for (it = _slist->begin(); it != itEnd; ++it)
        {
          d = (*it).second;
          fprintf(output, "%u %s %s %s %s %s %s\n", (UINT)d->id,
                  d->name.c_str(), d->ynotarg.c_str(),
                  d->rnotarg.c_str(), d->ttarg.c_str(),
                  d->rtarg.c_str(), d->ytarg.c_str());
        }
    }
  fclose(output);
}
void Socials::Load(void)
{
  if (!FileExists(SOCIALS_FILE))
    {
#ifdef NO_INIT_DEFAULTS
      InitializeDefaultSocials();
#else
      world->WriteLog("No socials file exists, and NO_INIT_DEFAULTS was enabled.", CRIT);
#endif
      return;
    }
  FILE* input = fopen(SOCIALS_FILE, "r");
  UINT id = 0;
  VNUM max_id = 0;
  UINT count, i;
  SOCIAL_DATA* data = NULL;
  char* name= new char[64+1];
  char* ynotarg = new char[64+1];
  char* rnotarg = new char[64+1];
  char* ttarg = new char[64+1];
  char* rtarg = new char[64+1];
  char* ytarg = new char[64+1];
  fscanf(input, "%u\n", &count);
  for (i = 0; i < count; ++i)
    {
      memset(name, 0, 64+1);
      memset(ynotarg, 0, 64+1);
      memset(rnotarg, 0, 64+1);
      memset(ttarg, 0, 64+1);
      memset(rtarg, 0, 64+1);
      memset(ytarg, 0, 64+1);
      fscanf(input, "%u %s %s %s %s %s %s\n", &id,
             name, ynotarg, rnotarg,
             ttarg, rtarg, ytarg);
      if (id > (UINT)max_id)
        {
          max_id=id+1;
        }

      data = new SOCIAL_DATA();
      data->name = name;
      data->ynotarg = ynotarg;
      data->rnotarg = rnotarg;
      data->ttarg = ttarg;
      data->rtarg = rtarg;
      data->ytarg = ytarg;
      data->id = id;
      (*_slist)[name] = data;
      data = NULL;
    }
  _socid = max_id;
  delete []name;
  delete []ynotarg;
  delete []rnotarg;
  delete []ttarg;
  delete []rtarg;
  delete []ytarg;
  fclose(input);
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
  Save();
#endif
}

void Socials::AddCommands(void)
{
  std::map <std::string, SOCIAL_DATA*>::iterator it;
  std::map <std::string, SOCIAL_DATA*>::iterator itEnd;

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
  world->WriteLog("Initializing socials.");
  Socials* soc = Socials::GetPtr();
  soc->Load();
  soc->AddCommands();
  world->events.AddCallback("Shutdown", socials_shutdown);
  world->events.AddCallback("Copyover", socials_shutdown);
  return true;
}
