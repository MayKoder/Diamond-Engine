#include"W_Assets.h"

#include"I_FileSystem.h"

W_Assets::W_Assets() : Window()
{
	name = "Assets";
}

W_Assets::~W_Assets()
{
}

void W_Assets::Draw()
{
	if (ImGui::Begin(name.c_str(), NULL/*, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize*/)) 
	{
		std::vector<std::string> names;
		std::vector<std::string> dirs;

		//Dirs returns directories inside the folder so we should use some kind of recursive get until dirs is empty
		FileSystem::GetAllFiles("Assets/", names, dirs);


		for (auto i = names.begin(); i != names.end(); ++i)
		{
			ImGui::Text((*i).c_str());
		}

		names.clear();
		dirs.clear();
	}
	ImGui::End();
}
