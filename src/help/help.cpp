/*
*help.cpp
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

#include "help.h"
#include "helpDb.h"
#include "helpNode.h"
#include "helpSection.h"
#include "../world.h"
#include "../command.h"
#include "../menu.h"
#include "../utils.h"

void InitializeHelp(void)
{
#ifdef MODULE_HELP
    world->WriteLog("Initializing help.");

    HelpDb* database=new HelpDb();
    world->AddProperty("help",(void*)database);

    if (!database->Load(std::string(HELP_FILE))) {
        world->WriteLog("Couldn't load the help database, this could be because you either don't have the permissions to load the file, or the file doesn't exist, in which case you have an empty database.");
    } else {
        world->WriteLog("Loaded the help database.");
    }

//    world->AddCommand("addsection","adds",RANK_GOD,0,0,do_addsection);
#endif
    return;
}

#ifdef MODULE_HELP
/*
CMD(do_addsection)
{
    UINT result=0;
    HelpDb* database=(HelpDb*)world->GetProperty("help");
    HelpSection* topic=NULL;

    if (!args.size()) {
        mobile->Message(MSG_ERROR,"Syntax: addtopic <topic>\nIf your topic is more than one word, you can put the name in quotes.");
        return;
    }

    result=database->GetSectionByName(args[1]);
    if (result!=(UINT)-1) {
        mobile->Message(MSG_ERROR,"That topic already exists.");
        return;
    }

    topic=new HelpSection(args[0]);
    database->AddHelpSection(topic);
    database->Save(HELP_FILE);
    mobile->Message(MSG_INFO,"OK, section added.");
}
*/
#endif
