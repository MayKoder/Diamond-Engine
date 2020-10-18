#include "W_Console.h"
#include "MMGui.h"
#include"Globals.h"

W_Console::W_Console() : collapseMode(true), scrollToBottom(false)
{
	name = "Console";
	AddLog("I want to die");
}

W_Console::~W_Console()
{
	logs.clear();
}

void W_Console::Draw()
{
	ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoResize*/ | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

	float offset = ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Clear").x - 7;
	ImGui::SetCursorPosX(offset);
	if (ImGui::Button("Clear"/*, ImGuiDir_Right*/))
	{
		logs.clear();
	}
	ImGui::SameLine();

	offset -= ImGui::CalcTextSize("Collapse").x + (ImGui::CalcTextSize("Clear").x / 2) - 7;
	ImGui::SetCursorPosX(offset);
	if (ImGui::Button("Collapse"/*, ImGuiDir_Right*/))
	{
		collapseMode = !collapseMode;
	}

	ImGui::GreySeparator();

	ImVec2 windowSize = ImGui::GetWindowSize();
	windowSize.y -= 63;
	windowSize.x -= 16;

	if (ImGui::BeginChild("##consolePrintArea", windowSize)) 
	{
		for (unsigned int i = 0; i < logs.size(); ++i)
		{

			ImGui::TextWrapped("[I]");
			ImGui::SameLine();
			ImGui::TextWrapped(logs[i].msg.c_str());

			if (logs[i].prints > 1)
			{
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize(std::to_string(logs[i].prints).c_str()).x);
				ImGui::Text("%i", logs[i].prints);
			}
		}

		if (scrollToBottom && !ImGui::IsWindowFocused())
		{
			ImGui::SetScrollHereY(1.0f);
			scrollToBottom = false;
		}
	}
	ImGui::EndChild();

	//if (ImGui::ListBoxHeader("##LogBox", windowSize)) {

	//	for (unsigned int i = 0; i < logs.size(); ++i)
	//	{
	//		ImGui::TextWrapped(logs[i].msg.c_str());

	//		if (logs[i].prints > 1)
	//		{
	//			ImGui::SameLine();
	//			ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize(std::to_string(logs[i].prints).c_str()).x);
	//			ImGui::Text("%i", logs[i].prints);
	//		}
	//	}
	//	ImGui::ListBoxFooter();
	//}

	ImGui::End();
}

void W_Console::AddLog(std::string s_msg)
{
	if (collapseMode && logs.size() >= 1)
	{
		if (logs[logs.size() - 1].msg == s_msg)
		{
			++logs[logs.size() - 1].prints;
			return;
		}
	}

	logs.push_back(LogMessage(s_msg));
	scrollToBottom = true;

}

LogMessage::LogMessage(std::string s_msg) : msg(s_msg), prints(1)
{
}

bool LogMessage::EqualsStr(const char* cmp)
{
	return (strcmp(msg.c_str(), cmp) == 0) ? true : false;
}
