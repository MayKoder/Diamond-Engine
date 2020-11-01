#pragma once

//ImGui Includes
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"

#include <Windows.h>

#include<string>


namespace ImGui
{

    inline bool CheckBoxFont(const char* name_, bool* pB_, const char* pOn_ = "[X]", const char* pOff_ = "[  ]")
    {
        if (*pB_)
        {
            ImGui::Text("%s %s", pOn_, name_);
        }
        else
        {
            ImGui::Text("%s %s", pOff_, name_);
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
        {
            *pB_ = !*pB_;
            return true;
        }
        return false;
    }

    inline bool RadioSelectable(const char* label, int* v, int v_button, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0))
    {
        if (ImGui::Selectable(label, *v == v_button, flags, size))
        {
            *v = v_button;
            return true;
        }
        return false;
    }

    inline void DummyWidget()
    {
        ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2.0f));
    }

    inline void DummyText()
    {
        ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeight()));
    }

    // menu item urls
    inline void MenuItemURL(const char* name_, const char* URL_)
    {
        std::string linkIconName = std::string("\t" + std::string(name_));
        if (ImGui::MenuItem(linkIconName.c_str()))
        {
            ShellExecute(0, 0, URL_, 0, 0, SW_SHOW);
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Open in browser\n%s", URL_);
        }
    }

};