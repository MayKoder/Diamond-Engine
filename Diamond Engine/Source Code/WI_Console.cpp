#include "WI_Console.h"
#include "MMGui.h"
#include"Globals.h"

W_Console::W_Console() : Window(), collapseMode(true), scrollToBottom(false)
{
	name = "Console";
}

W_Console::~W_Console()
{
	logs.clear();
}

void W_Console::Draw()
{
	if (ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoResize*/ | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar)) 
	{
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
			LogMessage* cLog = nullptr;
			ImVec4 labelColor(0.f, 0.f, 0.f, 0.f);

			for (unsigned int i = 0; i < logs.size(); ++i)
			{
				cLog = &logs[i];

				//ImGui::TextWrapped("[%c]", GetMsgType(cLog->lType));
				char labelLevel = GetMsgType(cLog->lType, labelColor);

				ImGui::TextColored(labelColor, "[%c]", labelLevel);

				ImGui::SameLine();
				ImGui::TextWrapped(cLog->msg.c_str());

				if (logs[i].prints > 1)
				{
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize(std::to_string(cLog->prints).c_str()).x);
					ImGui::Text("%i", cLog->prints);
				}
			}

			if (scrollToBottom && !ImGui::IsWindowFocused())
			{
				ImGui::SetScrollHereY(1.0f);
				scrollToBottom = false;
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void W_Console::AddLog(const char* s_msg, LogType _type)
{
	if (collapseMode && logs.size() >= 1)
	{
		if (logs[logs.size() - 1].msg == s_msg)
		{
			++logs[logs.size() - 1].prints;
			return;
		}
	}

	logs.push_back(LogMessage(s_msg, _type));
	scrollToBottom = true;

}

char W_Console::GetMsgType(LogType type, ImVec4 &lColor)
{	
	char ret = 'I';

	switch (type)
	{
		case LogType::L_NORMAL:
			ret = 'I';
			lColor = ImVec4(1.f, 1.f, 1.f, 1.f);
			break;

		case LogType::L_WARNING:
			ret = 'W';
			lColor = ImVec4(1.f, 1.f, 0.f, 1.f);
			break;

		case LogType::L_ERROR:
			ret = 'E';
			lColor = ImVec4(1.f, 0.f, 0.f, 1.f);
			break;
	}

	return ret;
}

LogMessage::LogMessage(std::string s_msg, LogType _type) : prints(1), lType(_type)
{
	msg = s_msg;
}

bool LogMessage::EqualsStr(const char* cmp)
{
	return (strcmp(msg.c_str(), cmp) == 0) ? true : false;
}
