#include"WI_Assets.h"

#include"IM_FileSystem.h"
#include"MO_ResourceManager.h"
#include"MO_Input.h"
#include"MO_Editor.h"

W_Assets::W_Assets() : Window(), selectedFile(nullptr)
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
		DrawFileTree(EngineExternal->moduleResources->assetsRoot);
		DrawFileTree(EngineExternal->moduleResources->meshesLibraryRoot);

		if (selectedFile != nullptr && /*ImGui::IsWindowHovered() &&*/ EngineExternal->moduleInput->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN) 
		{
			if (EngineExternal->moduleResources->GetTypeFromLibraryExtension(selectedFile->importPath.c_str()) == Resource::Type::UNKNOWN && strcmp(selectedFile->dirName.c_str(), "Meshes") != 0) //TODO: Temporal check
			{
				EngineExternal->moduleEditor->SetSelectedGO(nullptr);

				selectedFile->DeletePermanent();
				selectedFile = nullptr;

				EngineExternal->moduleResources->PopulateFileArray();
			}

		}
	}


	ImGui::End();
}

void W_Assets::DrawFileTree(AssetDir& file)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

	if (!file.isDir || file.childDirs.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	if (selectedFile == &file) 
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	//if (node == EngineExternal->moduleEditor->GetSelectedGO())
	//	flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;
	bool nodeOpen = ImGui::TreeNodeEx(&file, flags, file.dirName.c_str());

	if (ImGui::IsItemClicked()) {
		selectedFile = &file;
	}


	if (!file.isDir) 
	{
		if (ImGui::BeginDragDropSource(/*ImGuiDragDropFlags_SourceNoDisableHover*/))
		{
			if (EngineExternal->moduleResources->GetTypeFromAssetExtension(file.importPath.c_str()) == Resource::Type::MODEL)
				ImGui::SetDragDropPayload("_MODEL", &file.metaFileDir, file.metaFileDir.length());
			else if (EngineExternal->moduleResources->GetTypeFromAssetExtension(file.importPath.c_str()) == Resource::Type::TEXTURE)
				ImGui::SetDragDropPayload("_TEXTURE", &file.metaFileDir, file.metaFileDir.length());
			else if (EngineExternal->moduleResources->GetTypeFromLibraryExtension(file.importPath.c_str()) == Resource::Type::MESH)
				ImGui::SetDragDropPayload("_MESH", &file.importPath, file.importPath.length());
			//TODO: File is generating .meta files for library meshes... fix

			ImGui::Text("Import asset: %s", file.metaFileDir.c_str());
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
