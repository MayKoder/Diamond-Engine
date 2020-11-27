#include"W_Assets.h"

#include"IM_FileSystem.h"
#include"MO_ResourceManager.h"

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
		DrawFileTree(EngineExternal->moduleResources->rootFile);
	}
	ImGui::End();
}

void W_Assets::DrawFileTree(AssetDir& file)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

	if (!file.isDir || file.childDirs.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	//if (node == EngineExternal->moduleEditor->GetSelectedGO())
	//	flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;
	bool nodeOpen = ImGui::TreeNodeEx(&file, flags, file.dirName.c_str());

	if (!file.isDir) 
	{
		if (ImGui::BeginDragDropSource(/*ImGuiDragDropFlags_SourceNoDisableHover*/))
		{
			ImGui::SetDragDropPayload("_ASSET", file.importPath.c_str(), file.importPath.length());

			ImGui::Text("Import asset: %s", file.importPath.c_str());
			ImGui::EndDragDropSource();
		}
	}

	bool showChildren = (!file.isDir || file.childDirs.size() == 0) ? false : nodeOpen;

	if (showChildren == true)
	{
		for (unsigned int i = 0; i < file.childDirs.size(); i++)
		{
			DrawFileTree(file.childDirs[i]);
		}
		ImGui::TreePop();
	}
}
