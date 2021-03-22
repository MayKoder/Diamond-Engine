#ifndef STANDALONE

#include "WI_Console.h"
#include "MMGui.h"
#include"Globals.h"

#include"Application.h"
#include"MO_Editor.h"
#include"RE_Texture.h"

W_Console::W_Console() : Window(), collapseMode(true), scrollToBottom(false)
{
	name = "Console";

	showMessage[0] = showMessage[1] = showMessage[2] = true;
}

W_Console::~W_Console()
{
	this->ClearLogs();
}

void W_Console::Draw()
{
	if (ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoResize*/ | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar)) 
	{
		if(ImGui::Checkbox("Info", &showMessage[0]))
			FilterLogs();
		ImGui::SameLine();
		if(ImGui::Checkbox("Warnings", &showMessage[1]))
			FilterLogs();
		ImGui::SameLine();
		if(ImGui::Checkbox("Errors", &showMessage[2]))
			FilterLogs();
		ImGui::SameLine();

		float offset = ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Clear").x - 7;
		ImGui::SetCursorPosX(offset);
		if (ImGui::Button("Clear"/*, ImGuiDir_Right*/))
		{
			for (size_t i = 0; i < logs.size(); i++)
			{
				logs[i].msg.clear();
			}
			logs.clear();

			for (size_t i = 0; i < visibleLogs.size(); i++)
			{
				visibleLogs[i].msg.clear();
			}
			visibleLogs.clear();
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

			for (unsigned int i = 0; i < visibleLogs.size(); ++i) //Rendering all logs is dumb, need to find a way to ignore out-of-view logs
			{
				cLog = &visibleLogs[i];

				ImGui::Image(GetMsgType(cLog->lType), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0));

				ImGui::SameLine();
	
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
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
	LogMessage message(std::string(s_msg), _type);

	if (collapseMode && logs.size() >= 1)
	{
		if (logs[logs.size() - 1].msg == s_msg)
		{
			++logs[logs.size() - 1].prints;
			if (showMessage[(int)_type])
			{
				++visibleLogs[visibleLogs.size() - 1].prints;
			}
			return;
		}
	}

	logs.push_back(message);

	if (showMessage[(int)_type])
	{
		visibleLogs.push_back(message);
	}

	scrollToBottom = true;
}

void W_Console::FilterLogs()
{
	visibleLogs.clear();

	for (size_t i = 0; i < logs.size(); i++)
	{
		if (showMessage[(int)logs[i].lType])
		{
			visibleLogs.push_back(logs[i]);
		}
	}
}

void W_Console::ClearLogs()
{
	logs.clear();
	visibleLogs.clear();
}

ImTextureID W_Console::GetMsgType(LogType type)
{	
	ImTextureID ret = 0;

	switch (type)
	{
		case LogType::L_NORMAL:
			ret = (ImTextureID)EngineExternal->moduleEditor->editorIcons.GetIconTextureID("INFO");
			break;

		case LogType::L_WARNING:
			ret = (ImTextureID)EngineExternal->moduleEditor->editorIcons.GetIconTextureID("WARNING");
			break;

		case LogType::L_ERROR:
			ret = (ImTextureID)EngineExternal->moduleEditor->editorIcons.GetIconTextureID("ERROR");
			break;
	}

	return ret;
}

LogMessage::LogMessage(std::string& s_msg, LogType _type) : prints(1), lType(_type)
{
	msg = s_msg;
}

bool LogMessage::EqualsStr(const char* cmp)
{
	return (strcmp(msg.c_str(), cmp) == 0) ? true : false;
}

#endif // !STANDALONE