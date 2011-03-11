#ifndef TELNET_H
#define TELNET_H
#include "conf.h"
#include "mud.h"
#define TELNET_IAC '\xFF'
#define TELNET_WILL '\xFB'
#define TELNET_WONT '\xFC'
#define TELNET_DO '\xFD'
#define TELNET_DONT '\xFE'
#define TELNET_SB '\xFA'
#define TELNET_SE '\xF0'
#define TELNET_GA '\xF9'
//options
#define TELNET_ECHO '\x01'
#define TELNET_NOP '\xF1'
#define TELNET_AYT '\XF6'
#define TELNET_COMPRESS2 (char)86
const char TELNET_COMPRESS2_STR []
= {TELNET_IAC,TELNET_SB,TELNET_COMPRESS2,TELNET_IAC,TELNET_SE};
const char TELNET_ECHO_OFF[] = {TELNET_IAC, TELNET_WILL, TELNET_ECHO, '\0'};
const char TELNET_ECHO_ON[] = {TELNET_IAC, TELNET_WONT, TELNET_ECHO, '\0'};
#endif
