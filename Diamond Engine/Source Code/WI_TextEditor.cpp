#include "WI_TextEditor.h"
#include"IM_FileSystem.h"

W_TextEditor::W_TextEditor() : Window(), txtName(nullptr) /*: texColorBuffer(-1)*/
{
	name = "Text Editor"; //No lng definition for C# :(

	SetTextFromFile("CSSolution/Assembly-CSharp/Core.cs");
}

W_TextEditor::~W_TextEditor()
{

}

void W_TextEditor::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	if (ImGui::Begin(name.c_str(), NULL /*| ImGuiWindowFlags_NoResize*//*, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse*/))
	{
		txtEditor.Render(txtName);
	}
	ImGui::End();
	ImGui::PopStyleVar();

}

void W_TextEditor::SetTextFromFile(const char* path)
{
	txtName = path;

	std::string test = FileSystem::FileToText(path);
	txtEditor.SetText(test);
}
