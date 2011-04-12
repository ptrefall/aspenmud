#include <vector>
#include <string>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "olc.h"
#include "mud.h"
#include "conf.h"
#include "world.h"
#include "entity.h"
#include "player.h"
#include "utils.h"
#include "menu.h"
#include "command.h"
#include "editor.h"

#ifdef OLC
BOOL InitializeOlc(void)
{
  world->WriteLog("Initializing OLC.");
  world->commands.AddCommand(new CMDOedit());
  return true;
}

CMDOedit::CMDOedit()
{
  SetName("oedit");
  AddAlias("oed");
  SetAccess(RANK_BUILDER);
}
BOOL CMDOedit::Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd)
{
  if (!args.size())
    {
      mobile->Message(MSG_ERROR, "You must provide the name of an object to edit.");
      return false;
    }

  Entity* obj = world->MatchObject(args[0], mobile);
  if (!obj)
    {
      mobile->Message(MSG_ERROR, "That object wasn't found.");
      return false;
    }

  std::vector <struct OLC_DATA*> entries;
  std::vector <struct OLC_DATA*>::iterator it, itEnd;
  Menu* m = NULL;
  int count;

  obj->ListOlcs(entries);
  if (!entries.size())
    {
      mobile->Message(MSG_INFO, "That object doesn't have any OLC entries associated with it, exiting OEdit.");
      return false;
    }

  m = new Menu(mobile);
  itEnd = entries.end();
  for (it = entries.begin(), count = 1; it != itEnd; ++it, count++)
    {
      m->AddItem((*it)->name, olc_menu_cb, (void*)obj, count);
    }
  m->Attach();

  return true;
}

/*
*This is our menu handler.
*Basically all we need to do is create a couple structures to hold data (which gets messy), create the input structure to attach to sockets, and pass that in.
*The input handler will ask for input and get the result, then pass it to the olc callback
*which was already specified, after converting it.
*/
MENU(olc_menu_cb)
{
  Entity* obj = (Entity*)args; //the object we are editing.
  MENU_DATA* option = NULL; //the option chosen in the menu.
  OLC_DATA* olc = NULL; //the olc entry found on the object being edited.

  option = menu->GetDataByIndex(subitem); //gets the pointer to the menu option, given the number (subitem) that the user chose.
  olc = obj->GetOlcByIndex(subitem); //gets the olc item associated with the object.
  OLC_IN_DATA*arg = new OLC_IN_DATA(); //the input data that gets passed around while the user is editing.

  arg->obj = obj;
  arg->mobile = mob;
  arg->menudata = option;
  arg->olc = olc;
  if (arg->olc->type == EDITOR)
    {
//we need to handle this hear, otherwise we have problems when the user enters the editor because they have to input a command.
      (arg->olc->callback)(arg->obj, arg->mobile, NULL);
      return;
    }
  in_data* sdata = new in_data();
  sdata->args = (void*)arg;
  sdata->handle = new OlcInput();
  mob->GetSocket()->SetInput(sdata);
  mob->Message(MSG_INFO, olc->prompt);
}

void OlcInput::Input(void* arg, const std::string &input)
{
  OLC_IN_DATA* data= (OLC_IN_DATA*)arg;
  switch(data->olc->type)
    {
    default:
    {
      data->mobile->Message(MSG_ERROR, "Invalid input type specified, or not handled.");
      world->WriteLog("Invalid input type handled at: "+WHERE()+" for: "+data->olc->name+", on object "+data->obj->GetName()+".");
      break;
    }
    case NUM:
    {
      std::string::const_iterator it;
      int result = 0;
      for (it = input.begin(); it != input.end(); it++)
        {
          if (!isdigit((*it)))
            {
              data->mobile->Message(MSG_ERROR, "Invalid input.");
              break;
            }
        }

      result = atoi(input.c_str());
      Variant var = Variant(result);
      (data->olc->callback)(data->obj, data->mobile, &var);
      break;
    }
    case STRING:
    {
      Variant var= Variant(input);
      (data->olc->callback)(data->obj, data->mobile, &var);
      break;
    }
    }
  _sock->ClearInput();
  data->menudata->menu->ShowMenu();
  delete data;
}

void OlcString(Entity* ed, Player* mob, const Variant* input, boost::function<std::string ()> get, boost::function<void (const std::string&)> set)
{
  if (input->Typeof() != VAR_STR)
    {
      mob->Message(MSG_ERROR, "Invalid input.");
      return;
    }

  set(input->GetStr());
}

void SaveString(EventArgs* args, void* caller, boost::function<void (const std::string&)> set)
{
  OneArg* arg = (OneArg*)args;
  Editor* ed = (Editor*)arg->_arg;
  std::string str;
  std::vector<std::string>::iterator it, itEnd;

  std::vector<std::string>* lines = ed->GetLines();
  itEnd = lines->end();
  for (it = lines->begin(); it != itEnd; ++it)
    {
      str += "\n" + (*it);
    }

  set(str);
}
void OlcEditor(Entity* ed, Player* mob, const Variant* input, boost::function<std::string ()> get, boost::function<void (const std::string&)> set)
{
  Editor* edit = new Editor();
  edit->SetArg(ed);
  std::vector<std::string> lines;
  std::vector<std::string>::iterator it, itEnd;

  edit->events.AddCallback("save", boost::bind(SaveString, _1, _2, set));

  Tokenize(get(), lines, "\n");
  itEnd = lines.end();
  for (it = lines.begin(); it != itEnd; ++it)
    {
      edit->Add((*it), true);
    }

  edit->EnterEditor(mob);
}
#endif
