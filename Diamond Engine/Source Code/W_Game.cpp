#include "W_Game.h"

W_Game::W_Game() : Window()
{
	name = "Game";
}

W_Game::~W_Game()
{
}

void W_Game::Draw()
{
	//TODO: Could we call this from the base Window class? Like with components? Maybe to avoid duplicated code?
	if (ImGui::Begin(name.c_str(), NULL/*, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize*/))
	{
	}
	ImGui::End();
}
