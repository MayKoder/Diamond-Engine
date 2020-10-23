#include"W_About.h"
#include"OpenGL.h"
#include "SDL/include/SDL.h"
#include "EngineVersion.h"
#include "MMGui.h"

//TODO: DevIL version is missing

W_About::W_About()
{
	name = "About";
}

W_About::~W_About()
{
}

void W_About::Draw()
{
	ImGui::Begin(name.c_str(), NULL/*, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize*/);
	//ImGui::Separator();

	ImGui::TextWrapped("Diamond Engine v%s", EngineVersion::GetVersion().c_str());
	ImGui::NewLine();
	ImGui::TextWrapped("The new AAA engine for indie studios (or some shit like that)");
	ImGui::TextURL("by Miquel Suau Gonzalez (MayKoder)", "https://github.com/maykoder", 0, 0);

	ImGui::NewLine();
	ImGui::TextWrapped("3rd Party Libraries used:");

	//TODO: Versions should be stored on app start and not called every framne
	std::string printVersion;

	SDL_version ver;
	SDL_GetVersion(&ver);
	printVersion = "SDL v" + std::to_string(ver.major) + '.' + std::to_string(ver.minor) + '.' + std::to_string(ver.patch);
	ImGui::BulletText(""); ImGui::TextURL(printVersion.c_str(), "https://libsdl.org/index.php", 1, 0);

	printVersion = "ImGui  v" + std::string(ImGui::GetVersion());
	ImGui::BulletText(""); ImGui::TextURL(printVersion.c_str(), "https://github.com/ocornut/imgui", 1, 0);

	//TODO: Versions wont change, store versions in some sort of vector or array of strings
	ImGui::BulletText("OpenGL version supported: v%s", glGetString(GL_VERSION));
	ImGui::BulletText("Glew version: v%s", glewGetString(GLEW_VERSION));
	ImGui::BulletText("GLSL: v%s", glGetString(GL_SHADING_LANGUAGE_VERSION));

	ImGui::BulletText(""); ImGui::TextURL("Parson: v1.1.0 [No real time version available]", "https://github.com/kgabis/parson", 1, 0);
	ImGui::BulletText(""); ImGui::TextURL("MathGeoLib: v1.5 [No real time version available]", "https://github.com/juj/MathGeoLib", 1, 0);

	ImGui::NewLine(); ImGui::NewLine();
	ImGui::TextWrapped("License:");
	ImGui::NewLine();
	ImGui::TextWrapped("MIT License");
	ImGui::TextWrapped("Copyright(c) 2020 Miquel Suau Gonzalez");
	ImGui::Dummy(ImVec2(0.0f, 1.0f));
	ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copy \nof this softwareand associated documentation files (the 'Softwar'), to deal \nin the Software without restriction, including without limitation the rights \nto use, copy, modify, merge, publish, distribute, sublicense, and /or sell \ncopies of the Software, and to permit persons to whom the Software is \nfurnished to do so, subject to the following conditions:");
	ImGui::Dummy(ImVec2(0.0f, 1.0f));
	ImGui::TextWrapped("The above copyright noticeand this permission notice shall be included in all \ncopies or substantial portions of the Software.");
	ImGui::Dummy(ImVec2(0.0f, 1.0f));
	ImGui::TextWrapped("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR \nIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\nFITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\nLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\nSOFTWARE.");

	ImGui::End();
}
