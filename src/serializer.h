/*
*A basic set of helper functions for serializing to and from XML.
*/
#ifndef SERIALIZER_H
#define SERIALIZER_H
#include "mud.h"
#include "conf.h"
#include "exception.h"
#include <string>
#include <tinyxml.h>
class FileLoadException:public Exception
{
public:
    FileLoadException(const std::string &msg):Exception(msg) { }
};

#endif