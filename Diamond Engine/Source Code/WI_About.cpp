#ifndef STANDALONE

#include"WI_About.h"
#include"OpenGL.h"
#include "SDL/include/SDL.h"
#include "EngineVersion.h"
#include "MMGui.h"

#include"PhysFS/include/physfs.h"
#include"Assimp/include/version.h"

#include"ImTextEditor/TextEditor.h"

//TODO: DevIL version is missing

W_About::W_About() : Window()
{
	name = "About";
	
	PHYSFS_Version physVersionStc;
	PHYSFS_getLinkedVersion(&physVersionStc);

	physVersion = std::to_string(physVersionStc.major) + '.' + std::to_string(physVersionStc.minor) + '.' + std::to_string(physVersionStc.patch);
	assimpVersion = std::to_string(aiGetVersionMajor()) + '.' + std::to_string(aiGetVersionMinor()) + '.' + std::to_string(aiGetVersionRevision());
}

W_About::~W_About()
{
}

void W_About::Draw()
{
	if (ImGui::Begin(name.c_str(), NULL/*, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize*/)) 
	{
		//ImGui::Separator();

		ImGui::TextWrapped("Diamond Engine v%s", EngineVersion::GetVersion().c_str());
		ImGui::NewLine();

		//TODO: Could load this from json or txt
		ImGui::TextWrapped("Diamond Engine is a fast C++ 3D game engine, developed with SDL2 and OpenGL. The engine targets optimal memory management, fast starting and loading speeds and simple use. This will provide a fast, simple and powerful tool to prototype good looking games the fastest way possible.");
		ImGui::TextURL("Made by Miquel Suau Gonzalez (MayKoder)", "https://github.com/maykoder", 0, 0);

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

		printVersion = "PhysFS  v" + physVersion;
		ImGui::BulletText(""); ImGui::TextURL(printVersion.c_str(), "https://icculus.org/physfs/", 1, 0);

		printVersion = "Assimp  v" + assimpVersion;
		ImGui::BulletText(""); ImGui::TextURL(printVersion.c_str(), "https://www.assimp.org/", 1, 0);		
		
		ImGui::BulletText(""); ImGui::TextURL("DevIL  v1.8.0 [No real time version available]", "http://openil.sourceforge.net/", 1, 0);

		//TODO: Versions wont change, store versions in some sort of vector or array of strings
		printVersion = "OpenGL version supported: v";
		printVersion += (const char*)glGetString(GL_VERSION);
		ImGui::BulletText(""); ImGui::TextURL(printVersion.c_str(), "https://www.opengl.org/", 1, 0);

		printVersion = "Glew version: v";
		printVersion += (const char*)glewGetString(GLEW_VERSION);
		ImGui::BulletText(""); ImGui::TextURL(printVersion.c_str(), "http://glew.sourceforge.net/", 1, 0);

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

	}
	ImGui::End();
}

#endif // !STANDALONE