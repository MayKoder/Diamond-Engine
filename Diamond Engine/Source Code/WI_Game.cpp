#ifndef STANDALONE

#include "WI_Game.h"
#include "CO_Camera.h"
#include"Globals.h"

#include"DETime.h"
#include "SDL/include/SDL_mouse.h"

W_Game::W_Game() : Window(), targetCamera(nullptr)
{
	name = "Game";

	aspect[0] = 16;
	aspect[1] = 9;
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
		if (targetCamera != nullptr && targetCamera->resolvedFBO.GetFrameBuffer() != 0) {
			//LOG(LogType::L_WARNING, "Frame buffer game id: %d", targetCamera->framebuffer);
			//TODO: Dont modify aspect ratio every frame
			if(targetCamera->camFrustrum.type == FrustumType::PerspectiveFrustum)
				targetCamera->SetAspectRatio(ImGui::GetContentRegionAvail().x / ImGui::GetContentRegionAvail().y);
			else {
				targetCamera->camFrustrum.orthographicWidth = ImGui::GetContentRegionAvail().x / targetCamera->orthoSize;
				targetCamera->camFrustrum.orthographicHeight = ImGui::GetContentRegionAvail().y / targetCamera->orthoSize;
			}

			//float w = ImGui::GetWindowSize().x;
			//float h = (9 * w) / 16;
			ImGui::Image((ImTextureID)targetCamera->resolvedFBO.GetTextureBuffer(), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
		}
		if (ImGui::IsWindowHovered() && DETime::state == GameState::PLAY && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) 
			SDL_SetRelativeMouseMode(SDL_TRUE);

		//ImGui::SetCursorPos(ImVec2(10, 50));
		//if (ImGui::DragFloat2("Aspect Ratio", aspect, 0.2f, 0.f, 30.f))
		//{
		//	targetCamera->SetAspectRatio(aspect[0] / aspect[1]);
		//}

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

#endif // !STANDALONE