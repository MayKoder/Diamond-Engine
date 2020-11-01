#include "EngineVersion.h"

/*Get engine version*/
const std::string EngineVersion::GetVersion()
{
	std::string ret(std::to_string(DIAMONDENGINE_VERSION));
	ret.insert(MAJOR_VERSION_LENGHT, ".");
	ret.insert(MAJOR_VERSION_LENGHT + MINOR_VERSION_LENGHT, ".");

	return ret;
}