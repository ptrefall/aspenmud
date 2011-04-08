//mud constants and values:
#ifndef MUD_H
#define MUD_H
#include <boost/function.hpp> //this is needed so we can forward declare our olc typedef.
//if there is a way around this, I would love to know!

//used to show where an exception was thrown:
#define WHERE() std::string(" In ")+std::string(__FUNCTION__)+std::string(" at ")+std::string(__FILE__)+std::string(".")
//typedefs:
typedef int FLAG; //bitfield flags
typedef int VNUM;
typedef int BOOL;
typedef unsigned int UINT;
typedef unsigned int DWORD;

//forward declarations
//I wanted to avoid these, but it seems I have no choice
class World;
class Entity;
class Socket;
class Player;
class Component;
class Variant;

extern World* world;
//olc forward declarations
struct OLC_DATA;
enum OLC_INPUT_TYPE {NUM, STRING, BOOLEAN, EDITOR};
typedef void (*FP_INPUT)(Entity*, Player*, const Variant*);
typedef boost::function<void (Entity*, Player*, const Variant*)> OLCFUNC;
//input defines
#define OLC_INPUT(name)\
void name(Entity* ed, Player* mob, const Variant* input)
#define COLC_INPUT(c, name)\
void c::name(Entity* ed, Player* mob, const Variant* input)
#endif
