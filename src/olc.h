/*
*Olc mechenism used for registering
*Certain objects with OLC.
*/
#ifndef OLC_H
#define OLC_H
#include <string>
#include <boost/function.hpp>
#include "mud.h"
#include "conf.h"
#include "command.h"
#include "menu.h"
#include "editor.h"

#ifdef OLC
/*
*This is the basic structure that gets added every time you add an olc entry to an object.
*/
struct OLC_DATA
{
  std::string name; //the name of the option.
  std::string prompt;
  OLC_INPUT_TYPE type;
  OLCFUNC callback;
};

/*
*This is the information that gets passed around when the user is modifying an actual object. It gets messy because we need pointers to so much extra information.
*/
struct OLC_IN_DATA
{
  Entity* obj; //the object that is currently being edited.
  OLC_DATA* olc; //the pointer to the olc entry from the object being edited.
  Player*mobile; //the player actually editing this object.
  MENU_DATA* menudata; //the calling menu for this option.
};

BOOL InitializeOlc(void);

/*
*Our oedit command.
*Called when a builder wishes to edit an object.
*/
class CMDOedit:public Command
{
public:
  CMDOedit();
  BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};

/*
*This is our callback for when a user accesses the olc menu.
*/
MENU(olc_menu_cb);

/*
*This is our basic input handler class that accepts olc input.
*/
class OlcInput:public InputHandle
{
public:
  void Input(void* arg, const std::string &input);
};

/*
*These are generic functions that allow you to not have to worry about writing a callback function per every olc entry.
*The purpose is to allow you to provide these functions as a callback, while also providing a getter and a setter which can be used to get/set the data.
*/
/*
*Accepts a string and sets it back on the object. Useful for things like name, plural, etc.
*Params: [in] the object to edit.
*[in] the player doing the editing.
*[in] the variant being passed with the data.
*[in] a function object holding the getter.
*[in] A function object holding the setter.
*/
void OlcString(Entity* ed, Player* mob, const Variant* input, boost::function<std::string ()> get, boost::function<void (const std::string&)> set);
/*
*Shows a ToggleMenu with player ranks.
*Param: [in] The object you're editing, if any.
*Param: [in] the player doing the editing.
*Param: [in] NULL.
*Param: [in] the getter to retrieve the flag.
*Param: [in] the setter for setting the flag.
*/
void OlcPlayerFlag(Entity* ed, Player* mob, const Variant* input, boost::function<FLAG ()> get, boost::function<void (FLAG)> set);
/*
*This is the event that is used to save a specified string to an object.
*We can load the string when the editor is created, but when the editor saves we need a callback to set it on the object.
*/
void SaveString(EventArgs* args, void* caller, boost::function<void (const std::string&)> set);
/*
*Accepts a string, passes it to an editor and sets it when the editor saves.
*Params: [in] the object to edit.
*[in] the player doing the editing.
*[in] the variant being passed with the data.
*[in] a function object holding the getter.
*[in] A function object holding the setter.
*/
void OlcEditor(Entity* ed, Player* mob, const Variant* input, boost::function<std::string ()> get, boost::function<void (const std::string&)> set);
#endif
#endif
