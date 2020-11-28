#include "WI_Game.h"
#include "CO_Camera.h"
#include"Globals.h"

W_Game::W_Game() : Window(), targetCamera(nullptr)
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
		//Can't use if(true) return; because we need to call ImGui::End();
		if (targetCamera != nullptr && targetCamera->framebuffer != 0) {
			//LOG(LogType::L_WARNING, "Frame buffer game id: %d", targetCamera->framebuffer);
			//TODO: Dont modify aspect ratio every frame
			targetCamera->SetAspectRatio(ImGui::GetContentRegionAvail().x / ImGui::GetContentRegionAvail().y);
			ImGui::Image((ImTextureID)targetCamera->texColorBuffer, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
		}

	}
	ImGui::End();
}

void W_Game::SetTargetCamera(C_Camera* _cam)
{
	targetCamera = _cam;
}

C_Camera* W_Game::GetTargetCamera()
{
	return targetCamera;
}
