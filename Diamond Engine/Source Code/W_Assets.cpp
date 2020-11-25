#include"W_Assets.h"

#include"I_FileSystem.h"

W_Assets::W_Assets() : Window()
{
	name = "Assets";
}

W_Assets::~W_Assets()
{
	files.clear();
}

void W_Assets::PopulateFileArray()
{
	//Dirs returns directories inside the folder so we should use some kind of recursive get until dirs is empty
	FileSystem::GetAllFilesRecursive("Assets/", files);
}

void W_Assets::Draw()
{
	if (ImGui::Begin(name.c_str(), NULL/*, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize*/)) 
	{
		for (auto i = files.begin(); i != files.end(); ++i)
		{
			ImGui::Text((*i).c_str());
		}
	}
	ImGui::End();
}
