/*
*This set of functions provides functions for matching an object in the player's inventory, in a list of objects,
*or in the player's general location.
*/
#ifndef MATCH_H
#define MATCH_H
#include "mud.h"
#include "conf.h"
#include "entity.h"
#include <string>
#include <list>
#include <vector>
#include <utility>

/*
*Match return codes.
*/
enum MATCH_RETURN {M_SUCCESS, //success
                   M_FAIL, //failed without any reason.
                   M_BADNUM //there were some, but not enough for the number provided. e.g: 5.sword when you only have 2.
                  };
#define EXPLODE_MATCH_PAIR(pair, ret, obj)(ret = pair.first; obj = pair.second)

/*
*Matches an object in a list.
*Param: [in] the name of the object to find.
*param: [in] a pointer to the list of objects to match against.
*Return: An std::pair containing the return code and a pointer to the object.
*/
std::pair<MATCH_RETURN, Entity*> MatchObjectInList(const std::string &name, std::list<Entity*> *olist);
std::pair<MATCH_RETURN, Entity*> MatchObjectInList(const std::string &name, std::vector<Entity*> *olist);
#endif
