#include "MO_Renderer3D.h"
#include "MaykMath.h"
#include "MMGui.h"

#include "MO_Window.h"
#include "MO_Camera3D.h"
#include "MO_Editor.h"
#include "MO_Scene.h"
#include "MO_Input.h"
#include "MO_GUI.h"

#include "RE_Mesh.h"
#include "RE_Texture.h"
#include "mmgr/mmgr.h"

#include"WI_Game.h"

#include"GameObject.h"

#include"CO_MeshRenderer.h"
#include"CO_Camera.h"
#include"CO_Transform.h"

#include"Primitive.h"
#include"MathGeoLib/include/Geometry/Triangle.h"

#include"IM_TextureImporter.h"			//Delete this

#ifdef _DEBUG
#pragma comment (lib, "MathGeoLib/libx86/MGDebug/MathGeoLib.lib")
#else
#pragma comment (lib, "MathGeoLib/libx86/MGRelease/MathGeoLib.lib")
#endif

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib") /* link Microsoft OpenGL lib   */


ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled), str_CAPS(""),
vsync(false), wireframe(false), gameCamera(nullptr)
{
	GetCAPS(str_CAPS);
	/*depth =*/ cull = lightng = color_material = texture_2d = true;

	//framebuffer = texColorBuffer = rbo = 0;
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
	context = SDL_GL_CreateContext(App->moduleWindow->window);
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
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.1f);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendEquation(GL_FUNC_ADD);
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 0.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].diffuse.Set(0.05f, 0.05f, 0.05f, 1.0f);
		lights[0].SetPos(0.0f, 2.0f, 2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
	}

	//Generate texture
	for (int i = 0; i < SQUARE_TEXTURE_W; i++) {
		for (int j = 0; j < SQUARE_TEXTURE_H; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &checkersTexture);
	glBindTexture(GL_TEXTURE_2D, checkersTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SQUARE_TEXTURE_W, SQUARE_TEXTURE_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);	
	glBindTexture(GL_TEXTURE_2D, 0);


	// Projection matrix for
	OnResize(App->moduleWindow->s_width, App->moduleWindow->s_height);

	std::vector<std::string> faces = {
		"EngineIcons/Skybox/right.png",
		"EngineIcons/Skybox/left.png",
		"EngineIcons/Skybox/top.png",
		"EngineIcons/Skybox/bottom.png",
		"EngineIcons/Skybox/front.png",
		"EngineIcons/Skybox/back.png"
	};

	TextureImporter::LoadCubeMap(faces, skybox);
	skybox.CreateGLData();
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{

#ifndef STANDALONE
	App->moduleCamera->editorCamera.StartDraw();

	//Light 0 on cam pos
	lights[0].SetPos(5, 5, 5);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

#endif // !STANDALONE

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{

	Grid p(0, 0, 0, 0);
	p.axis = true;

#ifndef STANDALONE
	p.Render();

	//TODO: This should not be here
	if (!renderQueue.empty()) 
	{
		for (size_t i = 0; i < renderQueue.size(); i++)
		{
			float distance = App->moduleCamera->editorCamera.camFrustrum.pos.DistanceSq(renderQueue[i]->globalOBB.pos);
			renderQueueMap.emplace(distance, renderQueue[i]);
		}

		//TODO: Make wireframe only affect scene window
		(wireframe) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		RenderWithOrdering();
		(wireframe) ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


	skybox.DrawAsSkybox(&App->moduleCamera->editorCamera);

	DebugLine(pickingDebug);
	App->moduleCamera->editorCamera.EndDraw();
#endif // !STANDALONE

	//Draw game camera
	if (gameCamera != nullptr) 
	{
		gameCamera->StartDraw();

		lights[0].SetPos(5, 5, 5);

		for (uint i = 0; i < MAX_LIGHTS; ++i)
			lights[i].Render();

		if (!renderQueue.empty())
		{
			for (size_t i = 0; i < renderQueue.size(); i++)
			{
				float distance = gameCamera->camFrustrum.pos.DistanceSq(renderQueue[i]->globalOBB.pos);
				renderQueueMap.emplace(distance, renderQueue[i]);
			}

			RenderWithOrdering(true);
		}

		skybox.DrawAsSkybox(gameCamera);
		App->moduleGui->RenderCanvas2D();
		gameCamera->EndDraw();
	}


#ifndef STANDALONE
	App->moduleEditor->Draw();
#endif // !STANDALONE
	
	//TEMPORAL: Delete here so you can call mouse picking from scene window, should not be here in the future
	ClearAllRenderData();

	App->moduleScene->LoadHoldScene();

	SDL_GL_SwapWindow(App->moduleWindow->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG(LogType::L_NORMAL, "Destroying 3D Renderer");
	skybox.ClearMemory();

	SDL_GL_DeleteContext(context);
	ClearAllRenderData();

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);
	//ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);

	App->moduleWindow->s_width = width;
	App->moduleWindow->s_height = height;

#ifndef STANDALONE
	App->moduleCamera->editorCamera.ReGenerateBuffer(width, height);
#endif // !STANDALONE


	if (gameCamera != nullptr) 
		gameCamera->ReGenerateBuffer(width, height);

}

#ifndef STANDALONE
void ModuleRenderer3D::OnGUI()
{
	if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
	{

		//TODO: Store all this info as const char* and dont call the functions every frame
		SDL_version ver;
		SDL_GetVersion(&ver);
		ImGui::Text("SDL Version: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%d.%d.%d", ver.major, ver.minor, ver.patch);
		ImGui::Text("OpenGL Version: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", glGetString(GL_VERSION));
		ImGui::TextWrapped("All external library versions can be found in the 'About' window with links to their pages.");

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

		// Memory --------------------
		sMStats stats = m_getMemoryStatistics();

		ImGui::Text("Total Reported Mem: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%u bytes", stats.totalReportedMemory);
		ImGui::Text("Total Actual Mem: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%u bytes", stats.totalActualMemory);
		ImGui::Text("Peak Reported Mem: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%u bytes", stats.peakReportedMemory);
		ImGui::Text("Peak Actual Mem: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%u bytes", stats.peakActualMemory);

		ImGui::Checkbox("Enable V-Sync", &vsync);
			////Use Vsync
			//if (SDL_GL_SetSwapInterval(static_cast<int>(vsync)) < 0)
			//	LOG(LogType::L_WARNING, "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		ImGui::SameLine();
		ImGui::Checkbox("Wireframe Mode", &wireframe);

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

		ImGui::Separator();

		if (ImGui::DragFloat4("Ambient: ", &lights[0].ambient, 0.01f, 0.f, 1.f))
			lights[0].Init();
		if(ImGui::DragFloat4("Diffuse: ", &lights[0].diffuse, 0.01f, 0.f, 1.f)) 
			lights[0].Init();

	}
}
#endif // !STANDALONE

void ModuleRenderer3D::DrawBox(float3* points, float3 color)
{
	glColor3fv(&color.x);
	glLineWidth(2.f);
	glBegin(GL_LINES);

	//Draw plane
	glVertex3fv(&points[0].x);
	glVertex3fv(&points[2].x);
	glVertex3fv(&points[2].x);
	glVertex3fv(&points[6].x);
	glVertex3fv(&points[6].x);
	glVertex3fv(&points[4].x);
	glVertex3fv(&points[4].x);
	glVertex3fv(&points[0].x);

	glVertex3fv(&points[0].x);
	glVertex3fv(&points[1].x);
	glVertex3fv(&points[1].x);
	glVertex3fv(&points[3].x);
	glVertex3fv(&points[3].x);
	glVertex3fv(&points[2].x);
	glVertex3fv(&points[4].x);
	glVertex3fv(&points[5].x);

	glVertex3fv(&points[6].x);
	glVertex3fv(&points[7].x);
	glVertex3fv(&points[5].x);
	glVertex3fv(&points[7].x);
	glVertex3fv(&points[3].x);
	glVertex3fv(&points[7].x);
	glVertex3fv(&points[1].x);
	glVertex3fv(&points[5].x);

	glEnd();
	glLineWidth(1.f);
	glColor3f(1.f, 1.f, 1.f);
}

void ModuleRenderer3D::RayToMeshQueueIntersection(LineSegment& ray)
{
	pickingDebug = ray;

	std::map<float, C_MeshRenderer*> canSelect;
	float nHit = 0;
	float fHit = 0;

	bool selected = false;
	for (std::vector<C_MeshRenderer*>::iterator i = renderQueue.begin(); i != renderQueue.end(); ++i)
	{
		if (ray.Intersects((*i)->globalAABB, nHit, fHit))
			canSelect[nHit] = (*i);
	}


	//Add all meshes with a triangle hit and store the distance from the ray to the triangle, then pick the closest one
	std::map<float, C_MeshRenderer*> distMap;
	for(auto i = canSelect.begin(); i != canSelect.end(); ++i)
	{
		const ResourceMesh* _mesh = (*i).second->GetRenderMesh();
		if (_mesh)
		{
			LineSegment local = ray;
			local.Transform((*i).second->GetGO()->transform->globalTransform.Inverted());

			if (_mesh->vertices_count >= 9) //TODO: Had to do this to avoid squared meshes crash
			{
				for (uint index = 0; index < _mesh->indices_count; index += 3)
				{
					float3 pA(&_mesh->vertices[_mesh->indices[index] * VERTEX_ATTRIBUTES]);
					float3 pB(&_mesh->vertices[_mesh->indices[index + 1] * VERTEX_ATTRIBUTES]);
					float3 pC(&_mesh->vertices[_mesh->indices[index + 2] * VERTEX_ATTRIBUTES]);

					Triangle triangle(pA, pB, pC);

					float dist = 0;
					if (local.Intersects(triangle, &dist, nullptr))
						distMap[dist] = (*i).second;

				}
			}
		}
	}
	canSelect.clear();

#ifndef STANDALONE
	if (distMap.begin() != distMap.end())
	{
		App->moduleEditor->SetSelectedGO((*distMap.begin()).second->GetGO());
		selected = true;
	}


	//If nothing is selected, set selected GO to null
	if(!selected)
		App->moduleEditor->SetSelectedGO(nullptr);
#endif // !STANDALONE
	distMap.clear();
}

void ModuleRenderer3D::RenderWithOrdering(bool rTex)
{
	if (renderQueueMap.empty())
		return;

	for (auto i = renderQueueMap.rbegin(); i != renderQueueMap.rend(); ++i)
	{
		// Get the range of the current key
		auto range = renderQueueMap.equal_range(i->first);

		// Now render out that whole range
		for (auto d = range.first; d != range.second; ++d)
			d->second->RenderMesh(rTex);
	}

	renderQueueMap.clear();
}

void ModuleRenderer3D::DebugLine(LineSegment& line)
{
	glColor3f(1.f, 0.f, 0.f);
	glLineWidth(2.f);
	glBegin(GL_LINES);
	glVertex3fv(&pickingDebug.a.x);
	glVertex3fv(&pickingDebug.b.x);
	glEnd();
	glLineWidth(1.f);
	glColor3f(1.f, 1.f, 1.f);
}

/*Get SDL caps*/
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

C_Camera* ModuleRenderer3D::GetGameRenderTarget() const
{
	return gameCamera;
}

void ModuleRenderer3D::SetGameRenderTarget(C_Camera* cam)
{
	gameCamera = cam;

#ifndef STANDALONE
	W_Game* gWindow = dynamic_cast<W_Game*>(App->moduleEditor->GetEditorWindow(EditorWindow::GAME));
	if (gWindow != nullptr && gameCamera != nullptr)
		gWindow->SetTargetCamera(gameCamera);
#endif // !STANDALONE

	if (gameCamera != nullptr)
		gameCamera->ReGenerateBuffer(App->moduleWindow->s_width, App->moduleWindow->s_height);
}

void ModuleRenderer3D::ClearAllRenderData()
{
	renderQueueMap.clear();
	renderQueue.clear();
}
