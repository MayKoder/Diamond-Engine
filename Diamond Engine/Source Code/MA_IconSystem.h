#include<map>
#include<string>
#include"DEResource.h"
class ResourceTexture;
typedef unsigned int GLuint;


class IconManager
{
public:
	IconManager();
	~IconManager();
	void CleanUp();
	void LoadPreDefinedIcons();

	void LoadEditorIcon(const char* iconID, const char* path);

	GLuint GetIconTextureID(const char* iconID);
	GLuint GetIconTextureID(Resource::Type iconType);

	inline int GetIconCount() { return iconCount; }

private:
	//We should move on using a spritesheet and just use an [id, vec4] rect system
	std::map<std::string, ResourceTexture*> icons; //Is char* better? would it even work?
	int iconCount;

};