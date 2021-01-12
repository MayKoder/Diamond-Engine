#pragma once

//ImGui Includes
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

#include <Windows.h>

#include<string>

//TODO WARNING: Maybe create a .cpp file for this?
namespace ImGui
{
    void CenterNextElement(ImVec2 windowSize, float widthRatio);
    bool MiddleButton(const char* title, ImVec2 windowSize, float widthRatio);

    void GreySeparator();

    void AddUnderLine(ImColor col_);
    void TextURL(const char* name_, const char* URL_, uint8_t SameLineBefore_, uint8_t SameLineAfter_);
    ImGuiID DockSpaceOverViewportCustom(ImGuiViewport* viewport, ImGuiDockNodeFlags dockspace_flags, ImVec2 position, ImVec2 size, const ImGuiWindowClass* window_class);
    ImVec2 GetButtonSize(const char* text, const ImVec2 size_arg = ImVec2(0.f, 0.f));
}