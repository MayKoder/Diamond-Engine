#include "W_Hierarchy.h"

#include "Globals.h"
#include "GameObject.h"
#include "M_Scene.h"
#include "M_Editor.h"
#include "Application.h"
#include "W_Inspector.h"
#include "ModuleInput.h"

W_Hierarchy::W_Hierarchy(M_Scene* _scene) : Window(), cSceneReference(_scene)
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
		if (cSceneReference != nullptr && cSceneReference->root != nullptr)
		{
			DrawGameObjectsTree(cSceneReference->root, false);
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

	//if (IsHighlighted(node)) //TODO
	//	flags |= ImGuiTreeNodeFlags_Selected;

	bool nodeOpen = ImGui::TreeNodeEx(node, flags, node->name.c_str());

	if (drawAsDisabled)
		ImGui::PopStyleColor();

	if (ImGui::IsItemClicked() && !node->IsRoot())
	{
		dynamic_cast<W_Inspector*>(EngineExternal->moduleEditor->GetEditorWindow(EditorWindow::INSPECTOR))->selectedGO = node;
	}

	node->showChildren = (node->children.size() == 0) ? false : nodeOpen;

	if (node->showChildren == true)
	{

		for (unsigned int i = 0; i < node->children.size(); i++)
		{
			DrawGameObjectsTree(node->children[i], drawAsDisabled);
		}
		ImGui::TreePop();
	}
}
