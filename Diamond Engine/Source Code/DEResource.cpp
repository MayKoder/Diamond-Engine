#include "DEResource.h"
#include"Globals.h"

Resource::Resource(int _uid, Resource::Type _type) : uid(_uid), assetsFile(""), libraryFile(""), type(_type),
referenceCount(0)
{

}

Resource::~Resource()
{
	uid = 0;
	assetsFile.clear();
	libraryFile.clear();
	type = Type::UNKNOWN;

	if (referenceCount != 0)
		LOG(LogType::L_NORMAL, "DELETING RESOURCE WITH MULTIPLE REFERENCE COUNTS");

	referenceCount = 0;
}

void Resource::SetAssetsPath(const char* _aPath)
{
	assetsFile = _aPath;
}

void Resource::SetLibraryPath(const char* _lPath)
{
	libraryFile = _lPath;
}
