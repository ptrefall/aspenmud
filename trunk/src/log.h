/*
*log.h
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


#ifndef HEADER_LOG
#define HEADER_LOG
#include <string>
#include <cstdio>
#include "mud.h"
#include "conf.h"

/*
*Logger class.
*Used to log messages to both console and files.
*/
class Log
{
    FILE* out;
public:
    Log();
    ~Log();
    /*
    *Opens a log file.
    *Params: [in] The name of the file to open.
    */
    void Open(std::string name);
    /*
    *Writes the data to the console and the file if possible.
    *Param: [in] The message to write.
    */
    void Write(const std::string &data,LOG_LEVEL l=INFORM);
};
#endif
