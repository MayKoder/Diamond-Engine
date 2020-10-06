#pragma once

//ImGui Includes
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"

#include <Windows.h>

#include<string>

//WARNING: Add this to use URL based ImGui elements
namespace ImGui
{

    inline void GreySeparator() 
    {

        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Separator, ImVec4(0.3f, 0.3f, 0.3f, 1.f));
        ImGui::Separator();
        ImGui::PopStyleColor();
    }

    inline void AddUnderLine(ImColor col_)
    {
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        min.y = max.y;
        ImGui::GetWindowDrawList()->AddLine(min, max, col_, 1.0f);
    }

    // hyperlink urls
    inline void TextURL(const char* name_, const char* URL_, uint8_t SameLineBefore_, uint8_t SameLineAfter_)
    {
        if (1 == SameLineBefore_) { ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); }
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
        ImGui::Text(name_);
        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered())
        {
            if (ImGui::IsMouseClicked(0))
            {
                ShellExecute(0, 0, URL_, 0, 0, SW_SHOW);
            }
            AddUnderLine(ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
        }
        else
        {
            AddUnderLine(ImGui::GetStyle().Colors[ImGuiCol_Button]);
        }
        if (1 == SameLineAfter_) { ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x); }
    }


    inline ImGuiID DockSpaceOverViewportCustom(ImGuiViewport* viewport, ImGuiDockNodeFlags dockspace_flags, ImVec2 position, ImVec2 size, const ImGuiWindowClass* window_class)
    {
        if (viewport == NULL)
            viewport = GetMainViewport();

        SetNextWindowPos(position);
        SetNextWindowSize(size);
        SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags host_window_flags = 0;
        host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
        host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            host_window_flags |= ImGuiWindowFlags_NoBackground;

        char label[32];
        ImFormatString(label, IM_ARRAYSIZE(label), "DockSpaceViewport_%08X", viewport->ID);

        PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        Begin(label, NULL, host_window_flags);
        PopStyleVar(3);

        ImGuiID dockspace_id = GetID("DockSpace");
        DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags, window_class);
        End();

        return dockspace_id;
    }

}