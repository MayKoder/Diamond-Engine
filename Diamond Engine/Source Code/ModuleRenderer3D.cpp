#include "ModuleRenderer3D.h"
#include "MaykMath.h"
#include "MMGui.h"

#include "MeshArrays.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "M_Editor.h"
#include "M_Scene.h"

#include "FileSystem.h"
#include "Mesh.h"

#include "mmgr/mmgr.h"

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib") /* link Microsoft OpenGL lib   */

#pragma comment( lib, "DevIL/libx86/DevIL.lib" )
#include "DevIL\include\ilu.h"
#pragma comment( lib, "DevIL/libx86/ILU.lib" )
#include "DevIL\include\ilut.h"
#pragma comment( lib, "DevIL/libx86/ILUT.lib" )


ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled), str_CAPS(""),
vsync(false), wireframe(false)
{
	GetCAPS(str_CAPS);
	/*depth =*/ cull = lightng = color_material = texture_2d = true;

	framebuffer = texColorBuffer = rbo = 0;
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{
}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG(LogType::L_NORMAL, "Init: 3D Renderer context");
	bool ret = true;
	
	//ASK: Can i do this inside the MM namespace?
	MaykMath::Init();
	LOG(LogType::L_NORMAL, "Init: MaykMath");

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOG(LogType::L_ERROR, "OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	GLenum error = glewInit();
	if (error != GL_NO_ERROR)
	{
		LOG(LogType::L_ERROR, "Error initializing glew library! %s", SDL_GetError());
		ret = false;
	}
	else
	{
		LOG(LogType::L_NORMAL, "Init: Glew %s", glewGetString(GLEW_VERSION));
	}
	
	if(ret == true)
	{
		//Use Vsync
		if(SDL_GL_SetSwapInterval(static_cast<int>(vsync)) < 0)
			LOG(LogType::L_WARNING, "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			//gluErrorString
			LOG(LogType::L_ERROR, "Error initializing OpenGL! %s\n", glewGetErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			//ASK: Maybe glewGetErrorString is not the same as glutGerErrorString, so errors could be wrong
			LOG(LogType::L_ERROR, "Error initializing OpenGL! %s\n", glewGetErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			//ASK: Maybe glewGetErrorString is not the same as glutGerErrorString, so errors could be wrong
			LOG(LogType::L_ERROR, "Error initializing OpenGL! %s\n", glewGetErrorString(error));
			ret = false;
		}

		// Blend for transparency
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

		//Devil init
		ilInit();
		iluInit();
		ilutInit();
		ilutRenderer(ILUT_OPENGL);
	}

	//Generate texture
	//for (int i = 0; i < SQUARE_TEXTURE_W; i++) {
	//	for (int j = 0; j < SQUARE_TEXTURE_H; j++) {
	//		int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
	//		checkerImage[i][j][0] = (GLubyte)c;
	//		checkerImage[i][j][1] = (GLubyte)c;
	//		checkerImage[i][j][2] = (GLubyte)c;
	//		checkerImage[i][j][3] = (GLubyte)255;
	//	}
	//}


	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glGenTextures(1, &imgID);
	//glBindTexture(GL_TEXTURE_2D, imgID);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SQUARE_TEXTURE_W, SQUARE_TEXTURE_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
	//
	//glBindTexture(GL_TEXTURE_2D, 0);


	//VERTEX MODE
	//sizeof(float) * num_of_vertices * 3 = number of float inside the array

	//glGenBuffers(1, (GLuint*)&(my_id));
	//glBindBuffer(GL_ARRAY_BUFFER, my_id);
	///*glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_of_vertices * 3, cubeVertices, GL_STATIC_DRAW);*/
	//glBufferData(GL_ARRAY_BUFFER, sizeof(cubeSlowMesh), cubeSlowMesh, GL_STATIC_DRAW);
	
	//VERTEX X INDEX MODE
	//glGenBuffers(1, (GLuint*)&(my_vertices));
	//glBindBuffer(GL_ARRAY_BUFFER, my_vertices);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(MA_Pyramid_Vertices), MA_Pyramid_Vertices, GL_STATIC_DRAW);


	//glGenBuffers(1, (GLuint*)&(my_index));
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_index);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MA_Pyramid_Indices), MA_Pyramid_Indices, GL_STATIC_DRAW);

	//Generate scene buffers
	ReGenerateFrameBuffer(App->window->s_width, App->window->s_height);

	MeshLoader::FSInit();
	MeshLoader::EnableDebugMode();

	LOG(LogType::L_WARNING, "ModuleRender3D.cpp. line 185: Hardcoding a FBX load, remove ASAP");
	MeshLoader::ImportFBX("Assets/BakerHouse/BakerHouse.fbx", globalMeshes, App->moduleScene->root);


	// Projection matrix for
	OnResize(App->window->s_width, App->window->s_height);

	
	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glClearColor(0.08f, 0.08f, 0.08f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	//Light 0 on cam pos
	lights[0].SetPos(5, 5, 5);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{

	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	////<------------ VERTEX MODE ----------------->
	//glPushMatrix();
	//glTranslatef(0.f, 0.0f, 1.5f);
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glBindBuffer(GL_ARRAY_BUFFER, my_id);
	//glVertexPointer(3, GL_FLOAT, 0, NULL);

	////Bind other buffers
	//glDrawArrays(GL_TRIANGLES, 0, sizeof(cubeSlowMesh) / sizeof(float) / 3);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glDisableClientState(GL_VERTEX_ARRAY);
	//glPopMatrix();
	////<------------ VERTEX MODE END ----------------->

	//<------------ VERTEX AND INDEX MODE ----------------->
	//glPushMatrix();
	//glTranslatef(0.f, 0.0f, 3.f);

	//glEnableClientState(GL_VERTEX_ARRAY);
	//glBindBuffer(GL_ARRAY_BUFFER, my_vertices);
	//glVertexPointer(3, GL_FLOAT, 0, NULL);

	////TODO: Make a buffer for the colors and try this
	////glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	////glVertexAttribPointer((GLint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_index);

	////Bind other buffers
	//glDrawElements(GL_TRIANGLES, (sizeof(MA_Pyramid_Indices) / sizeof(int)), GL_UNSIGNED_INT, NULL);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glDisableClientState(GL_VERTEX_ARRAY);
	//glPopMatrix();
	//<------------ VERTEX AND INDEX MODE END ----------------->

	App->moduleScene->UpdateGameObjects();

	//App->level->Draw();
	//if (debug_draw == true)
	//{
	//	BeginDebugDraw();
	//	App->DebugDraw();
	//	EndDebugDraw();
	//}
	//App->editor->Draw();		[DONE]

	//Change frame buffer to default window
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);

	glClearColor(0.05f, 0.05f, 0.05f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	App->moduleEditor->Draw();

	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG(LogType::L_NORMAL, "Destroying 3D Renderer");

	MeshLoader::FSDeInit();
	MeshLoader::DisableDebugMode();

	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &texColorBuffer);
	glDeleteRenderbuffers(1, &rbo);

	//BUG: This is a bug, texture buffers are shared, should only delete each
	//Buffer once, keep a vector of texture buffers
	//glDeleteTextures(1, &testMeshes[0]->textureID);

	for (unsigned int j = 0; j < globalTextures.size(); ++j)
	{
		glDeleteTextures(1, &globalTextures[j]);
	}
	globalTextures.clear();

	for (unsigned int i = 0; i < globalMeshes.size(); i++)
	{
		delete globalMeshes[i];
		globalMeshes[i] = nullptr;
	}
	globalMeshes.clear();

	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	App->window->s_width = width;
	App->window->s_height = height;
}

void ModuleRenderer3D::OnGUI()
{
	if (ImGui::CollapsingHeader("Rendering"))
	{

		//TODO: Store all this info as const char* and dont call the functions every frame
		SDL_version ver;
		SDL_GetVersion(&ver);
		ImGui::Text("SDL Version: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%d.%d.%d", ver.major, ver.minor, ver.patch);

		ImGui::GreySeparator();
		ImGui::Text("CPUs: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%d (Cache: %dkb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
		ImGui::Text("System RAM: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%.1fGb", SDL_GetSystemRAM() / 1000.f);

#pragma region Caps
		ImGui::Text("Caps:");
		ImGui::SameLine();

		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), str_CAPS.c_str());

#pragma endregion


		ImGui::GreySeparator();
		ImGui::Text("GPU: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), (const char*)glGetString(GL_VENDOR));
		ImGui::Text("Brand: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), (const char*)glGetString(GL_RENDERER));

		//#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
		//#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049
		//GLint total_mem_kb = 0;
		//glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_mem_kb);
		//total_mem_kb /= 1000;
		//GLint cur_avail_mem_kb = 0;
		//glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &cur_avail_mem_kb);
		//cur_avail_mem_kb /= 1000;

		// Memory --------------------
		sMStats stats = m_getMemoryStatistics();
		/*static int speed = 0;
		static std::vector<float> memory(100);
		if (++speed > 20)
		{
			speed = 0;
			if (memory.size() == 100)
			{
				for (uint i = 0; i < 100 - 1; ++i)
					memory[i] = memory[i + 1];

				memory[100 - 1] = (float)stats.totalReportedMemory;
			}
			else
				memory.push_back((float)stats.totalReportedMemory);
		}

		ImGui::PlotHistogram("##memory", &memory[0], memory.size(), 0, "Memory Consumption", 0.0f, (float)stats.peakReportedMemory * 1.2f, ImVec2(310, 100));*/

		ImGui::Text("Total Reported Mem: %u", stats.totalReportedMemory);
		ImGui::Text("Total Actual Mem: %u", stats.totalActualMemory);
		ImGui::Text("Peak Reported Mem: %u", stats.peakReportedMemory);
		ImGui::Text("Peak Actual Mem: %u", stats.peakActualMemory);
		//ImGui::Text("Accumulated Reported Mem: %u", stats.accumulatedReportedMemory);
		//ImGui::Text("Accumulated Actual Mem: %u", stats.accumulatedActualMemory);
		//ImGui::Text("Accumulated Alloc Unit Count: %u", stats.accumulatedAllocUnitCount);
		//ImGui::Text("Total Alloc Unit Count: %u", stats.totalAllocUnitCount);
		//ImGui::Text("Peak Alloc Unit Count: %u", stats.peakAllocUnitCount);

		if(ImGui::Checkbox("Enable V-Sync", &vsync))
		{
			//Use Vsync
			if (SDL_GL_SetSwapInterval(static_cast<int>(vsync)) < 0)
				LOG(LogType::L_WARNING, "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Wireframe Mode", &wireframe))
			(wireframe) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//if (ImGui::Checkbox("Depth Test", &depth))
		//	(depth) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
		ImGui::SameLine();
		if (ImGui::Checkbox("Cull face", &cull))
			(cull) ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

		if (ImGui::Checkbox("Lighting", &lightng))
			(lightng) ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
		ImGui::SameLine();
		if (ImGui::Checkbox("Color material", &color_material))
			(color_material) ? glEnable(GL_COLOR_MATERIAL) : glDisable(GL_COLOR_MATERIAL);

		if (ImGui::Checkbox("Texture 2D", &texture_2d))
			(texture_2d) ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);
	}
}

void ModuleRenderer3D::ReGenerateFrameBuffer(int w, int h)
{
	if(framebuffer > 0)
		glDeleteFramebuffers(1, &framebuffer);

	if (texColorBuffer > 0)
		glDeleteTextures(1, &texColorBuffer);

	if (rbo > 0)
		glDeleteRenderbuffers(1, &rbo);


	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG(LogType::L_ERROR, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleRenderer3D::CustomLoadImage(const char* fileName)
{
	//std::string test("Assets/BakerHouse/");
	//test += fileName;
	//SUPER ERROR TODO: ilutGLBindTexImage() SHOULD NOT 
	//BE CALLED EVERY MESH, SHOULD IT?
	ILuint imageID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	if (!ilLoadImage(fileName))
	{
		LOG(LogType::L_ERROR, "Image not loaded");
	}

	GLuint glID = ilutGLBindTexImage();
	for (unsigned int i = 0; i < globalMeshes.size(); i++)
	{
		//This should not be called every mesh like wtf
		globalMeshes[i]->textureID = glID;
	}

	ilDeleteImages(1, &imageID);
}

void ModuleRenderer3D::GetCAPS(std::string& caps)
{
	caps += (SDL_HasRDTSC()) ? "RDTSC," : "";
	caps += (SDL_HasMMX()) ? "MMX, " : "";
	caps += (SDL_HasSSE()) ? "SSE, " : "";
	caps += (SDL_HasSSE2()) ? "SSE2, " : "";
	caps += (SDL_HasSSE3()) ? "SSE3, " : "";
	caps += "\n";
	caps += (SDL_HasSSE41()) ? "SSE41, " : "";
	caps += (SDL_HasSSE42()) ? "SSE42, " : "";
	caps += (SDL_HasAVX()) ? "AVX, " : "";
	caps += (SDL_HasAltiVec()) ? "AltiVec, " : "";
	caps += (SDL_Has3DNow()) ? "3DNow, " : "";
}