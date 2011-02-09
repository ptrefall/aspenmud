/*
*editor.cpp
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


#include <sstream>
#include <vector>
#include <string>
#include "player.h"
#include "socket.h"
#include "utils.h"
#include "editor.h"

INPUT(editor_ed)
{
    if (input=="") {
        return;
    }

    Player* mobile;
    Editor* ed;
    int index=0;

    std::vector <std::string> tokens;

    mobile=sock->GetPlayer();
    ed=(Editor*)arg;
    Tokenize(input,tokens);

//a list of lines
    if ((tokens[0]=="l")||(tokens[0]=="list")) {
        if ((tokens.size()>1)&&(tokens[1]=="#")) {
            ed->List(true);
        } else {
            ed->List(false);
        }
        return;
    }
//insertion
    if ((tokens[0]=="i")||(tokens[0]=="insert")) {
        if (tokens.size()==1) {
            mobile->Message(MSG_ERROR,"You must provide the line number where you want to place the cursor.");
            return;
        }
        if (tokens[1]=="$") {
            ed->Insert(-1);
        } else {
            ed->Insert(atoi(tokens[1].c_str()));
        }
        return;
    }
//delete
    if ((tokens[0]=="delete")||(tokens[0]=="d")) {
        if (tokens.size()==1) {
            ed->Delete();
        }
        if (tokens.size()==2) {
            ed->Delete(atoi(tokens[1].c_str()));
        }
        if (tokens.size()==3) {
            if (tokens[2]=="$") {
                ed->Delete(atoi(tokens[1].c_str()),-1);
            } else {
                ed->Delete(atoi(tokens[1].c_str()),atoi(tokens[2].c_str()));
            }
        }
        return;
    }
//abort
    if (tokens[0]=="abort") {
        ed->Abort();
        return;
    }
//help
    if ((tokens[0]=="h")||(tokens[0]=="help")) {
        mobile->Message(MSG_INFO,"\t\tEditor help.");
        mobile->Message(MSG_INFO, "\"[text]: insert a line. using \" by its self will append a blank line.");
        mobile->Message(MSG_INFO,"i <index>: will place your cursor. Note: the insertion mark will appear on the line below your insertion point when you get a listing. i<$> will set the insertion mark at the end.");
        mobile->Message(MSG_INFO,"D|d<line>|d<start> <end>: delete lines. d by its self will delete the line where the insertion point is located. Providing one argument will delete a specified line. You may also delete multiple lines, providing the start and end lines. You can substitute the \'$\' in any of these to signify the end.");
        mobile->Message(MSG_INFO,"L <#>: will list the lines in the editor. Providing the optional \'#\' will show line numbers.");
        mobile->Message(MSG_INFO, "H: shows this help.");
        mobile->Message(MSG_INFO, "Q: Quit and prompt to save.");
        mobile->Message(MSG_INFO,"A: abort without saving.");
        mobile->Message(MSG_INFO, "s: Save changes.");
        return;
    }
//quit
    if ((tokens[0]=="quit")||(tokens[0]=="q")) {
        ed->Quit();
        return;
    }
//save
    if ((tokens[0]=="save")||(tokens[0]=="s")) {
        ed->Save();
        return;
    }
//append line
    if (input[0]=='\"') {
        if (input.length()==1) {
            ed->Add("\n");
            return;
        }
        if ((input[1]==' ')&&(input.length()>2)) {
            index=2;
        } else {
            index=1;
        }
//we strip off the quote and add the rest.
        ed->Add(input.substr(index,input.length()));
        return;
    }

    mobile->Message(MSG_ERROR, "Unknown editor command. Use \'h\' for help.");
}

Editor::Editor()
{
    _cursor=0;
    _dirty=false;
    _handler=NULL;
}
Editor::~Editor()
{
    if (_handler) {
        delete _handler;
    }
}

BOOL Editor::Load(void)
{
    return true;
}
void Editor::Save(void)
{
    _dirty=false;
    return;
}

void Editor::Quit(void)
{
    if (!_dirty) {
        _mobile->Message(MSG_INFO,"Exiting editor.");
        LeaveEditor();
    } else {
        _mobile->Message(MSG_ERROR,"You have not saved your text. Please use \'s\', to save changes, or \'a\', to abort without saving.");
    }
}
void Editor::Abort(void)
{
    _mobile->Message(MSG_INFO,"Aborting.");
    LeaveEditor();
}

void Editor::List(BOOL num)
{
    std::list <std::string>::iterator it;
    int i;
    std::stringstream st;

    if (!_lines.size()) {
        _mobile->Message(MSG_INFO, "Buffer is empty.");
        return;
    }

    for (i=1,it=_lines.begin(); it!=_lines.end(); i++,it++) {
        if (_cursor==(i-1)) {
            st << "||";
        }
        if (num) {
            st << i << " ";
        }
        st << (*it);
        _mobile->Message(MSG_LIST,st.str());
        st.str("");
    }
    if (_cursor==-1) {
        _mobile->Message(MSG_LIST,"||");
    }
}

void Editor::Add(const std::string &line)
{
    std::stringstream st;
    std::list <std::string>::iterator it;
    int i=0;

    if (_cursor==-1) {
        _lines.push_back(line);
        st << "Line " << _lines.size() << " added.";
        _mobile->Message(MSG_INFO,st.str());
    } else {
//the cursor is pointing to the insertion point; jump the iterator there and insert.
        for (it=_lines.begin(),i=0; i!=_cursor; i++,it++);
        _lines.insert(it,line);
        _mobile->Message(MSG_INFO,"Line inserted.");
        _cursor++;
    }
}

void Editor::Insert(int index)
{
    std::stringstream st;

    if ((index==-1)||(index>=(int)_lines.size())) {
        _cursor=-1;
        _mobile->Message(MSG_INFO,"Cursor append to end of text.");
        return;
    }

    index--; //so it will match up with our cursor.
    _cursor=index;
    st << "Insertion point set at line " << index+1 << ".";
    _mobile->Message(MSG_INFO,st.str());
}

void Editor::Delete(void)
{
    std::list <std::string>::iterator it;
    int i=0;

    if (!_lines.size()) {
        _mobile->Message(MSG_INFO,"Buffer is empty.");
        return;
    }
    if (_cursor==-1) {
        _mobile->Message(MSG_INFO,"Cursor is already at the end of the buffer.");
        return;
    }

    for (it=_lines.begin(),i=0; i!=_cursor; i++,it++);

    _lines.erase(it);
    _mobile->Message(MSG_INFO,"Line erased.");
}
void Editor::Delete(int index)
{
    std::list <std::string>::iterator it;
    int i=0;

    if ((index<=0)||(index>(int)_lines.size())) {
        _mobile->Message(MSG_INFO,"Line out of range.");
        return;
    }

    for (it=_lines.begin(),i=0; i!=index; i++,it++);

    _lines.erase(it);
    _mobile->Message(MSG_INFO,"Line erased.");
}
void Editor::Delete(int first, int second)
{
    int i=0;
    std::list <std::string>::iterator top,bottom;

    if (((first>second)&&(second!=-1))||(first==second)||(first<=0)||(second>(int)_lines.size())) {
        _mobile->Message(MSG_ERROR,"Invalid range.");
        return;
    }

    if (second==(int)_lines.size()) {
        second=-1;
    }

    for (top=_lines.begin(),i=0; i<first; top++,i++);
    if (second==-1) {
        bottom=_lines.end();
    } else {
        for (bottom=_lines.begin(),i=0; i<second; bottom++,i++);
    }

    _lines.erase(top,bottom);
    _mobile->Message(MSG_INFO,"Lines deleted.");
}

BOOL Editor::EnterEditor(Player* mobile)
{
    _mobile=mobile;
    _handler=new in_data();
    _handler->handle=editor_ed;
    _handler->args=NULL;
    _mobile->GetSocket()->SetInput(_handler);
    _mobile->Message(MSG_INFO,"Entering editor. use \'h\' for help.");
    return true;
}
void Editor::LeaveEditor()
{
    _mobile->GetSocket()->ClearInput();
    if (_handler) {
        delete _handler;
        _handler=NULL;
    }
}