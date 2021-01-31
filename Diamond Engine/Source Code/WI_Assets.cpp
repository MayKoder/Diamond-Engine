#ifndef STANDALONE

#include"WI_Assets.h"

#include"IM_FileSystem.h"
#include"MO_ResourceManager.h"
#include"MO_Input.h"
#include"MO_Editor.h"
#include"MO_MonoManager.h"
#include"RE_Texture.h"
#include"WI_TextEditor.h"

W_Assets::W_Assets() : Window(), selectedFile(nullptr)
{
	name = "Assets";
	displayFolder = &EngineExternal->moduleResources->assetsRoot;
}

W_Assets::~W_Assets()
{
}

void W_Assets::Draw()
{
	if (ImGui::Begin(name.c_str(), NULL/*, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize*/))
	{
		//if (ImGui::BeginChild("Test", ImVec2(70, 110), false, ImGuiWindowFlags_NoScrollbar))
		//{
		//	//ImGui::hove
		//	ImGui::Image((ImTextureID)1, ImVec2(70, 70));
		//	ImGui::TextWrapped("Hola.exe");
		//}
		//ImGui::EndChild();
		
		
		DrawFileTree(*displayFolder);
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

		if (ImGui::BeginPopupContextWindow())
		{

			if (ImGui::BeginMenu("Create C# Script"))
			{
				static char name[50] = "\0";

				ImGui::Text("Script path: "); ImGui::SameLine();
				ImGui::InputText("##Scriptname", name, sizeof(char) * 50);
				if (ImGui::Button("Create"))
				{
					std::string path = name;
					if(path.find('.') == path.npos)
						path += ".cs";

					if (path.find('.cs') != path.npos) {
						EngineExternal->moduleMono->CreateAssetsScript(path.c_str());
						name[0] = '\0';
					}

					ImGui::CloseCurrentPopup();
				}


				ImGui::EndMenu();
			}
			ImGui::EndPopup();
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

	if (file.isDir) 
	{
		ImGui::Image((ImTextureID)EngineExternal->moduleEditor->editorIcons[static_cast<int>(Icons::I_Folder)]->textureID, ImVec2(12, 12), ImVec2(0, 1), ImVec2(1, 0));
	}
	else {
		ImGui::Image((ImTextureID)EngineExternal->moduleEditor->editorIcons[static_cast<int>(Icons::I_Models)]->textureID, ImVec2(12, 12), ImVec2(0, 1), ImVec2(1, 0));
	}
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
			if (type == Resource::Type::SCRIPT /*|| type == Resource::Type::SHADER*/)
			{
				W_TextEditor* txtEditor = dynamic_cast<W_TextEditor*>(EngineExternal->moduleEditor->GetEditorWindow(EditorWindow::TEXTEDITOR));
				txtEditor->SetTextFromFile(file.importPath.c_str());
				//Load script text and open visual studio?
			}
		}
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
#endif // !STANDALONE

