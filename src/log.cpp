/*
*log.cpp
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


#include <cstdio>
#include <string>
#include <ctime>
#include <cstring>
#include "log.h"
Log::Log()
{
out = NULL;
}
Log::~Log()
{
    if (out) {
        fclose(out);
        out = NULL;
    }
}

void Log::Open(std::string name)
{
    if (!out) {
        out = fopen(name.c_str(), "a");
    }
}

void Log::Write(const std::string &data,LOG_LEVEL l)
{
    time_t curtime;
    char* stime=NULL;
    std::string level;
    struct tm* tinfo;

    if (out) {
        stime=new char[32];
        time(&curtime);
        tinfo=localtime(&curtime);

        if (stime==NULL) {
            return;
        }

        memset(stime, 0, 32);
        strftime(stime,32,"%x %X",tinfo);

        switch (l) {
        case INFORM:
            level="[INFO] ";
            break;
        case WARN:
            level="[WARNING] ";
            break;
        case ERR:
            level="[ERROR] ";
            break;
        case CRIT:
            level="[CRITICAL ERROR] ";
            break;
        case SCRIPT:
            level="[SCRIPT] ";
            break;
        case PLAYER:
            level="[player]";
            break;
        case CONNECTION:
            level="[CONNECTION]";
            break;
        default:
            break;
        }

        fprintf(out, "%s %s: %s.\n", level.c_str(), stime, data.c_str());
#ifdef LOG_CONSOLE
        if (MIN_LOG_LEVEL>=l) {
            printf("%s %s: %s\n",level.c_str(),stime,data.c_str());
        }
#endif

        if (stime) {
            delete []stime;
        }
    }
}
