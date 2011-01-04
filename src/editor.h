/*
*editor.h
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
 *Basic editor object
 */
#ifndef EDITOR_H
#define EDITOR_H
#include <list>
#include <string>
#include "mud.h"
#include "conf.h"
#include "player.h"
#include "socket.h"

class Editor
{
    Player* _mobile;
    std::list <std::string> _lines;
    int _cursor;
    BOOL _dirty;
    struct in_data *_handler;
public:
    Editor(void);
    virtual    ~Editor(void);
    virtual BOOL Load(void);
    virtual void Save(void);
    virtual void Abort(void);
    virtual void Quit(void);
    virtual void List(BOOL lnum);
    virtual void Add(const std::string &line);
    virtual void Insert(int index);
    virtual void Delete(void);
    virtual void Delete(int index);
    virtual void Delete(int first, int second);
    virtual BOOL EnterEditor(Player* mobile);
    virtual void LeaveEditor(void);
};

INPUT(editor_ed);
#endif
