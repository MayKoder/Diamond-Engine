#ifndef STANDALONE

#include "WI_Configuration.h"
#include "MMGui.h"

#include "Application.h"
#include "MaykMath.h"


W_Configuration::W_Configuration() : Window()
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

    if (ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize)) 
    {
	    //ImGui::SetNextTreeNodeOpen(true);
	    if (ImGui::CollapsingHeader("Application", ImGuiTreeNodeFlags_DefaultOpen))
	    {
		    ImGui::PlotHistogram("##frameRate", &fps_log[0], fps_log.size(), 0, "Framerate", 0.0f, 100.0f, ImVec2(310, 100));
		    ImGui::PlotHistogram("##miliseconds", &ms_log[0], ms_log.size(), 0, "Miliseconds", 0.0f, 100.0f, ImVec2(310, 100));
		    ImGui::SliderInt("FPS CAP", &EngineExternal->fpsCap, 10, 360);
		    ImGui::NewLine();
	    }

	    for (unsigned int i = 0; i < EngineExternal->list_modules.size(); ++i)
	    {
		    EngineExternal->list_modules[i]->OnGUI();
	    }


        //ImGuiStyle* ref = NULL;
        ImGuiStyle& style = ImGui::GetStyle();
        //static ImGuiStyle ref_saved_style;

	    if (ImGui::CollapsingHeader("Style Settings"))
	    {
            static ImGuiTextFilter filter;
            filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

            static ImGuiColorEditFlags alpha_flags = 0;

            ImGui::BeginChild("##colors", ImVec2(0, 300), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NavFlattened);
            ImGui::PushItemWidth(-160);
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                const char* name = ImGui::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                ImGui::PushID(i);
                ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
                ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                ImGui::TextUnformatted(name);
                ImGui::PopID();
            }
            ImGui::PopItemWidth();
            ImGui::EndChild();
	    }
    }
	ImGui::End();
}

void W_Configuration::UpdateInfoLogs()
{

	//Do not use ImGui::GetFrameRate()
	float currentFPS = floorf(EngineExternal->GetFrameRate())/*ImGui::GetIO().Framerate*/;
	float currentMS = (1000.f * EngineExternal->GetDT());

	//FPS Vector Update
	MaykMath::FixedVectorPushBack(fps_log, currentFPS);

	//MS Vector Update
	MaykMath::FixedVectorPushBack(ms_log, currentMS);

}

#endif // !STANDALONE