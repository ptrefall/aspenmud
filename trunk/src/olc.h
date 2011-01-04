/*
*olc.h
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
*Olc mechenism used for registering
*Certain objects with OLC.
*/

#ifndef OLC_H
#define OLC_H
#include <string>
#include "mud.h"
#include "conf.h"
#include "command.h"
#include "menu.h"
//#include "editor.h"

#ifdef OLC
//input defines
#define OLC_INPUT(name)\
void name(Entity* ed, Player* mob, const Variant* input)

struct OLC_DATA {
    std::string name; //the name of the option.
    std::string prompt;
    OLC_INPUT_TYPE type;
    FP_INPUT callback;
};

struct OLC_IN_DATA {
    Entity* obj;
    OLC_DATA* olc;
    Player*mobile;
    MENU_DATA* menu;
};

BOOL InitializeOlc(void);
class CMDOedit:public Command
{
public:
    CMDOedit();
    BOOL Execute(const std::string &verb, Player* mobile,std::vector<std::string> &args,int subcmd);
};
MENU(olc_menu_cb);
INPUT(olc_input);
#endif
#endif
