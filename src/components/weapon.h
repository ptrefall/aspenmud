/*
The basic weapon class.
*/
#ifndef WEAPON_H
#define WEAPON_H
#include "../mud.h"
#include "../conf.h"
#include "../component.h"

//weapon flags
#define WF_TWOHAND 1<<0 //requires two hands to wield.
#define WF_VORPAL 1<<1 //extra critical
#define WF_VAMPIRIC 1<<2 //heals
class Weapon:public Component
{
