/*
*serializer.hpp
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


#include <string>
#include <cstdlib>
#include <cstring>
#include <cstdio>
/*
*Serialization library
*Serializes files in binary form, with minimal overhead.
*/
#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP
#include "mud.h"
#include "conf.h"
#include "exception.h"

enum SERIALIZATION_MODE {NONE, WRITE, READ};
class Serializer
{
    FILE* _bin;
    SERIALIZATION_MODE _mode;
public:
    Serializer(FILE* file,SERIALIZATION_MODE mode) {
        if (file==NULL) {
            throw(Exception("File pointer must not be NULL."));
            return;
        }
        if (mode==NONE) {
            throw(Exception("Mode must not be set to \'NONE\'."));
            return;
        }

        _mode=mode;
        _bin=file;
    }
    ~Serializer(void) {
        fclose(_bin);
    }

    template <class T>
    void Write(const T &data) {
        if (_mode!=WRITE) {
            throw(Exception("Can not write to a file whose mode is set to \'READ\'."));
            return;
        }
        fwrite(&data, sizeof(T), 1, _bin);
    }
    template <class T>
    void Read(T *r) {
        if (_mode!=READ) {
            throw(Exception("Can not read from a file whose mode is set to \'READ\'."));
            return;
        }
        if (fread(r, sizeof(T), 1, _bin)!=1) {
            throw(Exception("Invalid return value from fread."));
            return;
        }
    }
    void ReadBytes(char* buffer, unsigned int length) {
        if (_mode!=READ) {
            throw(Exception("Can not read from a file whose mode is set to \'READ\'."));
            return;
        }
        if (fread(buffer, sizeof(char), length, _bin)!=length) {
            throw(Exception("Invalid return value from fread."));
            return;
        }
    }

    void WriteString(const char* str) {
        size_t len=strlen(str);

        if (_mode!=WRITE) {
            throw(Exception("Can not write to a file whose mode is set to \'READ\'."));
            return;
        }
        if (fwrite(str, sizeof(char), len+1, _bin)!=len+1) {
            throw(Exception("Invalid write length."));
            return;
        }
    }
    void WriteString(const std::string &str) {
        WriteString((char*)str.c_str());
    }
    void WriteBytes(const char* buffer, unsigned int length) {
        if (_mode!=WRITE) {
            throw(Exception("Can not write to a file whose mode is set to \'READ\'."));
            return;
        }
        if (fwrite(buffer, sizeof(char), length, _bin)!=length) {
            throw(Exception("Invalid write length."));
            return;
        }
    }

    std::string ReadString(void) {
        std::string ret;
        char i;
        while (1) {
            i=fgetc(_bin);
            if (i==0) {
                break;
            }
            ret+=(char)i;
        }
        return ret;
    }

    template <class T>
    Serializer& operator <<(const T &data) {
        Write(data);
        return *this;
    }
    Serializer& operator <<(const std::string &str) {
        WriteString(str);
        return *this;
    }
    template <class T>
    Serializer& operator >>(T &data) {
        Read(&data);
        return *this;
    }
    Serializer& operator >>(std::string &data) {
        data=ReadString();
        return *this;
    }

};
#endif