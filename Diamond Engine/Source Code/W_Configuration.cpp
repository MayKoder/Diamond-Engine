#include "W_Configuration.h"


W_Configuration::W_Configuration() : displayWindow(false)
{
	name = "Configuration";
	fps_log.reserve(FPS_MS_LOG_MAXLENGHT);
	ms_log.reserve(FPS_MS_LOG_MAXLENGHT);
}

W_Configuration::~W_Configuration()
{
	ms_log.clear();
	fps_log.clear();
}

void W_Configuration::Draw()
{

	UpdateInfoLogs();

	ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Separator();

	//ImGui::SetNextTreeNodeOpen(true);
	if (ImGui::CollapsingHeader("Application"))
	{
		ImGui::PlotHistogram("##frameRate", &fps_log[0], fps_log.size(), 0, "Framerate", 0.0f, 100.0f, ImVec2(310, 100));
		ImGui::PlotHistogram("##miliseconds", &ms_log[0], ms_log.size(), 0, "Miliseconds", 0.0f, 100.0f, ImVec2(310, 100));
		ImGui::SliderInt("FPS CAP", &Engine->fpsCap, 1, 360);
		ImGui::NewLine();

		if(ImGui::Button("Display example")) 
		{
			displayWindow = !displayWindow;
		}

	}

	for (unsigned int i = 0; i < Engine->list_modules.size(); ++i)
	{
		Engine->list_modules[i]->OnGUI();
	}

	ImGui::End();

	if (displayWindow) 
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_MenuBarBg, ImVec4(0.f, 0.f, 0.f, 1.f));
		ImGui::ShowDemoWindow();
		ImGui::PopStyleColor();
	}

}

void W_Configuration::UpdateInfoLogs()
{

	//ASK: We should not use ImGui get framerate, we must get our own
	float currentFPS = ImGui::GetIO().Framerate;
	float currentMS = 1000.f * Engine->GetDT();

	//FPS Vector Update
	MaykMath::FixedVectorPushBack(fps_log, currentFPS);

	//MS Vector Update
	MaykMath::FixedVectorPushBack(ms_log, currentMS);

}



