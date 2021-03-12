#ifndef STANDALONE

#include"WI_Assets.h"
#include"WI_TextEditor.h"
#include"WI_Inspector.h"

#include"IM_FileSystem.h"
#include"IM_ShaderImporter.h"
#include"IM_MaterialImporter.h"
#include "IM_PrefabImporter.h"

#include"MO_ResourceManager.h"
#include"MO_Input.h"
#include"MO_Editor.h"
#include"MO_MonoManager.h"
#include"RE_Texture.h"

#include "MO_Scene.h"

#include "GameObject.h"

W_Assets::W_Assets() : Window(), selectedFile(nullptr)
{
	name = "Assets";

	displayFolder = &EngineExternal->moduleResources->assetsRoot;
	bigDisplayFolder = displayFolder;
}

W_Assets::~W_Assets()
{
}

void W_Assets::Draw()
{
	static float width = ImGui::GetContentRegionAvail().x * 0.45f;
	if (ImGui::Begin(name.c_str(), NULL/*, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize*/))
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar;
		ImGui::BeginChild("Tree", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.2f, ImGui::GetContentRegionAvail().y), false, window_flags);

		DrawFileTree(*displayFolder);
		DrawCreationWindow();
		DrawFileTree(EngineExternal->moduleResources->meshesLibraryRoot);
		DrawFileTree(EngineExternal->moduleResources->animationsLibraryRoot);
		ImGui::EndChild();

		if (selectedFile != nullptr && /*ImGui::IsWindowHovered() &&*/ EngineExternal->moduleInput->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN) 
		{ //This prevents mesh removal because mesh files have no dirName
			if (EngineExternal->moduleResources->GetTypeFromLibraryExtension(selectedFile->libraryPath.c_str()) != Resource::Type::UNKNOWN && strcmp(selectedFile->dirName.c_str(), "Meshes") != 0) 
			{
				EngineExternal->moduleEditor->SetSelectedGO(nullptr);

				selectedFile->DeletePermanent();
				selectedFile = nullptr;

				EngineExternal->moduleResources->PopulateFileArray();
			}
		}

		ImGui::SameLine();
		ImGui::BeginChild("Folder", ImVec2(0, ImGui::GetContentRegionAvail().y), true);
		DrawCurrentFolder();

		ImGui::EndChild();

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


	ImGui::Image((ImTextureID)EngineExternal->moduleEditor->editorIcons.GetIconTextureID(file.resourceType), ImVec2(12, 12), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::SameLine();

	bool nodeOpen = ImGui::TreeNodeEx(&file, flags, file.dirName.c_str());

	if (ImGui::IsItemClicked()) 
	{
		selectedFile = &file;

		//if (ImGui::IsMouseDoubleClicked(0) && file.isDir)
		//	displayFolder = &file;
		if (file.isDir == false && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left)) 
		{
			Resource::Type type = EngineExternal->moduleResources->GetTypeFromAssetExtension(file.importPath.c_str());
			if (type == Resource::Type::SCRIPT || type == Resource::Type::SHADER)
			{
				W_TextEditor* txtEditor = dynamic_cast<W_TextEditor*>(EngineExternal->moduleEditor->GetEditorWindow(EditorWindow::TEXTEDITOR));
				txtEditor->SetTextFromFile(file.importPath.c_str());
				//Load script text and open visual studio?
			}
			else if(type == Resource::Type::MATERIAL)
			{
				W_Inspector* inspector = dynamic_cast<W_Inspector*>(EngineExternal->moduleEditor->GetEditorWindow(EditorWindow::INSPECTOR));
				inspector->SetEditingResource(EngineExternal->moduleResources->RequestFromAssets(selectedFile->importPath.c_str()));
			}
		}
	}

	if (!file.isDir) 
	{
		if (ImGui::BeginDragDropSource(/*ImGuiDragDropFlags_SourceNoDisableHover*/))
		{
			SetFilePayload(file);
			ImGui::Text("Import asset: %s", file.metaFileDir.c_str());
			ImGui::EndDragDropSource();
		}
	}
	else
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_GAMEOBJECT"))
			{
				int uid = *(int*)payload->Data;

				GameObject* droppedGO = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, uid);

				std::string assets_path = file.importPath.c_str();
				assets_path += "/" + std::string(droppedGO->name) + ".prefab";
				droppedGO->prefabID = PrefabImporter::SavePrefab(assets_path.c_str(), droppedGO);
			}
			ImGui::EndDragDropTarget();
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

void W_Assets::DrawCreationWindow()
{
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::BeginMenu("Create C# Script"))
		{
			DrawCreationPopup("Script path: ", ".cs", std::bind(&M_MonoManager::CreateAssetsScript, EngineExternal->moduleMono, std::placeholders::_1));
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Create GLSL Shader"))
		{
			DrawCreationPopup("Shader path: ", ".glsl", ShaderImporter::CreateBaseShaderFile);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Create Material"))
		{
			DrawCreationPopup("Material path: ", ".mat", MaterialImporter::CreateBaseMaterialFile);
			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}
}

void W_Assets::DrawCreationPopup(const char* popDisplay, const char* dotExtension, std::function<void(const char*)> f)
{
	static char name[50] = "\0";

	ImGui::Text(popDisplay); ImGui::SameLine();

	std::string id("##");
	id += dotExtension;

	ImGui::InputText(id.c_str(), name, sizeof(char) * 50);
	if (ImGui::Button("Create"))
	{
		std::string path = name;
		if (path.find('.') == path.npos)
			path += dotExtension;

		//TODO: Check if the extension is correct, to avoid a .cs.glsl file
		if (path.find(dotExtension) != path.npos) 
		{ 
			f(path.c_str());
			name[0] = '\0';
		}

		ImGui::CloseCurrentPopup();
	}
}

void W_Assets::DrawPathButtons()
{
}

void W_Assets::DrawCurrentFolder()
{
	int cellSize = 70 + 20;
	int windowWidth = (int)ImGui::GetWindowContentRegionMax().x;

	int cellsPerRow = windowWidth / cellSize;
	int counter = 0;

	AssetDir* toChange = nullptr;

	if (bigDisplayFolder->parentDir != nullptr)
		if (ImGui::Button("Go back"))
			bigDisplayFolder = bigDisplayFolder->parentDir;

	DrawCreationWindow();

	for (std::vector<AssetDir>::const_iterator it = bigDisplayFolder->childDirs.begin(); it != bigDisplayFolder->childDirs.end(); ++it)
	{
		bool selected = selectedFile == it._Ptr;
		if(selected)
			ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(60, 110, 170, 255));

		if (ImGui::BeginChild(it->dirName.c_str(), ImVec2(70, cellSize), false, ImGuiWindowFlags_NoScrollbar))
		{
			ImVec4 backgroundColor = ImVec4(0,0,0,1);
			if (selected)
				backgroundColor = ImVec4(0, 0, 0, 0);


			//ImGui::PushID(it->dirName.c_str());
			if (ImGui::ImageButton((ImTextureID)EngineExternal->moduleEditor->editorIcons.GetIconTextureID(it->resourceType), ImVec2(70, 70), ImVec2(0, 1), ImVec2(1, 0), 0 ,backgroundColor)) 
			{
				if (selectedFile != it._Ptr) // Select unselected file 
				{
					selectedFile = it._Ptr; 
				}
				else if (it->isDir) //Open selected file if it is a folder
				{
					toChange = it._Ptr;
					selectedFile = nullptr;
				}
			}

			if (!it->isDir)
			{
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptBeforeDelivery))
				{
					SetFilePayload(*it._Ptr);
					ImGui::Text("Import asset: %s", it->importPath.c_str());
					ImGui::EndDragDropSource();
				}
			}
			else
			{
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_GAMEOBJECT"))
					{
						int uid = *(int*)payload->Data;

						GameObject* droppedGO = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, uid);

						std::string assets_path = it->importPath.c_str();
						assets_path += "/" + std::string(droppedGO->name) + ".prefab";
						droppedGO->prefabID = PrefabImporter::SavePrefab(assets_path.c_str(), droppedGO);
					}
					ImGui::EndDragDropTarget();
				}
			}

			ImGui::Text(it->dirName.c_str());
		}
		ImGui::EndChild();

		if(selected)
			ImGui::PopStyleColor();

		counter++;
		if (counter < cellsPerRow && it != bigDisplayFolder->childDirs.end() - 1)
			ImGui::SameLine(0.0f, 15.0f);
		else
			counter = 0;
	}
	if (toChange != nullptr) 
	{
		bigDisplayFolder = toChange;
		//ImGui::SetScrollHereY(1.0f);
	}
}
void W_Assets::SetFilePayload(AssetDir& file)
{
	switch (file.resourceType)
	{
	case  Resource::Type::MODEL:
		ImGui::SetDragDropPayload("_MODEL", &file.metaFileDir, file.metaFileDir.length());
		break;
	case  Resource::Type::TEXTURE:
		ImGui::SetDragDropPayload("_TEXTURE", &file.metaFileDir, file.metaFileDir.length());
		break;
	case  Resource::Type::MESH:
		ImGui::SetDragDropPayload("_MESH", &file.importPath, file.importPath.length());
		break;
	case  Resource::Type::MATERIAL:
		ImGui::SetDragDropPayload("_MATERIAL", &file.importPath, file.importPath.length());
		break;
	case  Resource::Type::SHADER:
		ImGui::SetDragDropPayload("_SHADER", &file.metaFileDir, file.metaFileDir.length());
		break;
	case  Resource::Type::ANIMATION:
		ImGui::SetDragDropPayload("_ANIMATION", &file.importPath, file.importPath.length());
		break;
	case  Resource::Type::FONT:
		ImGui::SetDragDropPayload("_FONT", &file.importPath, file.importPath.length());
		break;
	case  Resource::Type::PREFAB:
		ImGui::SetDragDropPayload("_PREFAB", &file.metaFileDir, file.metaFileDir.length());
		break;
	default:
		break;
	}
}
#endif // !STANDALONE

