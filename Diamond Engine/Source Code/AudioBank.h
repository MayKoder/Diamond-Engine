#pragma once
#ifndef __AUDIOBANKK_H__
#define __AUDIOBANKK_H__

#include <string>
#include <map>

#include "Globals.h"

typedef struct
{
	std::string						bank_name;
	bool							loaded_in_heap;
	std::map<uint64, std::string>	events;         // Sound and fxs
	std::map<uint64, std::string>	actions;		// Other events

}AudioBank;

#endif // !__AUDIOBANKK_H__