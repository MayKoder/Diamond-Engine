#ifndef STANDALONE

#include "WI_Hierarchy.h"

#include "Globals.h"
#include "GameObject.h"
#include "MO_Scene.h"
#include "MO_Editor.h"
#include "Application.h"
#include "WI_Inspector.h"
#include "MO_Input.h"
#include"AssetDir.h"

W_Hierarchy::W_Hierarchy(M_Scene* _scene) : Window(), cSceneReference(_scene), dropTarget(nullptr)
{
	name = "Hierarchy";
}

W_Hierarchy::~W_Hierarchy()
{
	cSceneReference = nullptr;
}


void W_Hierarchy::Draw()
{
	if (ImGui::Begin(name.c_str(), NULL /*| ImGuiWindowFlags_NoResize*/)) 
	{
		//if (ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left)) {
		//	dropTarget = nullptr;
		//}
		bool tree_open = false;
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected;

		if (EngineExternal->moduleScene->current_scene_name[0] == '\0') {
			tree_open = ImGui::TreeNodeEx("untitled scene", flags);}
		else {
			tree_open = ImGui::TreeNodeEx(EngineExternal->moduleScene->current_scene_name, flags); }

		if (tree_open)
		{
			if (cSceneReference != nullptr && cSceneReference->root != nullptr)
			{
				DrawGameObjectsTree(cSceneReference->root, false);
			}
			ImGui::TreePop();
		}

		if (ImGui::BeginPopupContextWindow())
		{
			EngineExternal->moduleEditor->DrawCreateMenu();
			if (ImGui::Selectable("Create Empty"))
			{
				GameObject* parent = (EngineExternal->moduleEditor->GetSelectedGO() != nullptr) ? EngineExternal->moduleEditor->GetSelectedGO() : EngineExternal->moduleScene->root;
				EngineExternal->moduleScene->CreateGameObject("Empty", parent);

				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void W_Hierarchy::SetCurrentScene(M_Scene* _scene)
{
	cSceneReference = _scene;
}

void W_Hierarchy::DrawGameObjectsTree(GameObject* node, bool drawAsDisabled)
{

	if (drawAsDisabled == false)
		drawAsDisabled = !node->isActive();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

	if (node->children.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	if (node == EngineExternal->moduleEditor->GetSelectedGO())
		flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;


	if (drawAsDisabled)
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);

	bool nodeOpen = ImGui::TreeNodeEx(node, flags, node->name.c_str());

	if (drawAsDisabled)
		ImGui::PopStyleColor();

	//Only can use if this is not the root node
	//ASK: Should the root node really be a gameobject? Problems with checks
	if (!node->IsRoot()) 
	{
		//Start drag for reparent
		if (ImGui::BeginDragDropSource(/*ImGuiDragDropFlags_SourceNoDisableHover*/))
		{
			ImGui::SetDragDropPayload("_GAMEOBJECT", node, sizeof(GameObject*));

			dropTarget = node;

			ImGui::Text("Change parent to...");
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			EngineExternal->moduleEditor->SetSelectedGO(node);
			if (EngineExternal->moduleEditor->GetSelectedAsset() != nullptr)
				EngineExternal->moduleEditor->SetSelectedAsset(nullptr);
		}
	}

	node->showChildren = (node->children.size() == 0) ? false : nodeOpen;
	
	//All nodes can be a drop target
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_GAMEOBJECT")) 
		{

			GameObject* dropGO = static_cast<GameObject*>(payload->Data);
			//memcpy(dropGO, payload->Data, payload->DataSize);

			dropTarget->ChangeParent(node);
			LOG(LogType::L_NORMAL, "%s", dropTarget->name.c_str());
			dropTarget = nullptr;
		}
		ImGui::EndDragDropTarget();
	}


	if (node->showChildren == true)
	{

		for (unsigned int i = 0; i < node->children.size(); i++)
		{
			DrawGameObjectsTree(node->children[i], drawAsDisabled);
		}
		ImGui::TreePop();
	}
}

#endif // !STANDALONE