#ifndef STANDALONE

#include "WI_Scene.h"

#include "MO_Renderer3D.h"
#include "MO_Window.h"
#include "MO_Camera3D.h"
#include "MO_Editor.h"
#include "MO_Scene.h"
#include "MO_Input.h"
#include "MO_ResourceManager.h"

#include"CO_Transform.h"
#include"GameObject.h"

#include "IM_PrefabImporter.h"

#include "COMM_Transform.h"

#include"MathGeoLib/include/Geometry/LineSegment.h"

W_Scene::W_Scene(Application* _app) : Window() /*: texColorBuffer(-1)*/, manipulatingGuizmo(false)
{
	name = "Scene";
	App = _app;

	operation = ImGuizmo::OPERATION::TRANSLATE;
	mode = ImGuizmo::MODE::WORLD;

	oldMat = float4x4::zero;
}

W_Scene::~W_Scene()
{

}

void W_Scene::InitSceneView()
{

}

void W_Scene::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	if (ImGui::Begin(name.c_str(), NULL /*| ImGuiWindowFlags_NoResize*//*, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse*/)) 
	{
		if (/*ImGui::IsWindowFocused() &&*/ ImGui::IsWindowHovered() && !ImGuizmo::IsUsing()) 
		{
			//TODO: Uncomment this, it's the editor camera input handler
			App->moduleCamera->ProcessSceneKeyboard();
		}

		ImVec2 size = ImGui::GetContentRegionAvail();

		App->moduleCamera->editorCamera.SetAspectRatio(ImGui::GetContentRegionAvail().x / ImGui::GetContentRegionAvail().y);
		ImGui::Image((ImTextureID)App->moduleCamera->editorCamera.resolvedFBO.GetTextureBuffer(), size, ImVec2(0, 1), ImVec2(1, 0));

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_MODEL"))
			{
				//Drop asset from Asset window to scene window
				std::string* name = (std::string*)payload->Data;

				//TODO: This is working only with textures now
				std::string libPath = EngineExternal->moduleResources->LibraryFromMeta((*name).c_str());
				EngineExternal->moduleScene->LoadModelTree(libPath.c_str());
				
				LOG(LogType::L_WARNING, "Model %s loaded to resource manager", (*name).c_str());
			}

			if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
			{
				//Drop asset from Asset window to scene window
				std::string* name = (std::string*)payload->Data;
				
				//TODO: Change selected mesh texture
				//EngineExternal->moduleResources->AssetsToScene((*name).c_str());
				LOG(LogType::L_WARNING, "Texture %s did nothing", (*name).c_str());
			}

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_PREFAB"))
			{
				std::string* name = (std::string*)payload->Data;

				std::string libraryPath = EngineExternal->moduleResources->LibraryFromMeta((*name).c_str());
				GameObject* rootObject = PrefabImporter::LoadPrefab(libraryPath.c_str());

				//if(rootObject != nullptr)
					//EngineExternal->moduleScene->
				LOG(LogType::L_WARNING, "Prefab %s loaded to resource manager", (*name).c_str());
			}

			ImGui::EndDragDropTarget();
		}

		int position = 10;
		ImGui::SetCursorPos(ImVec2(position, 30));
		if (ImGui::Button((mode == ImGuizmo::MODE::LOCAL) ? "LOCAL" : "WORLD"))
			(mode == ImGuizmo::MODE::LOCAL) ? mode = ImGuizmo::MODE::WORLD : mode = ImGuizmo::MODE::LOCAL;


		//Draw gizmo
		DrawGuizmo();

		if (ImGui::IsMouseClicked(0) && !ImGuizmo::IsUsing() && !App->moduleInput->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN)
		{
			ImVec2 position = ImGui::GetMousePos();
			ImVec2 normal = NormalizeOnWindow(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetFrameHeight(), ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - ImGui::GetFrameHeight(), position);
			normal.x = (normal.x - 0.5f) / 0.5f;
			normal.y = -((normal.y - 0.5f) / 0.5f);

			if (ImGui::IsWindowHovered())
			{
				LineSegment picking = App->moduleCamera->editorCamera.camFrustrum.UnProjectLineSegment(normal.x, normal.y);
				//ImGui::SetCursorPos(ImVec2(100, 30));
				//ImGui::Text("%f, %f", normal.x, normal.y);
				App->moduleRenderer3D->RayToMeshQueueIntersection(picking);
				App->moduleEditor->SetSelectedAsset(nullptr);
			}
		}

	}
	ImGui::End();
	ImGui::PopStyleVar();

}

ImVec2 W_Scene::NormalizeOnWindow(float x, float y, float w, float h, ImVec2 point)
{
	ImVec2 normalizedPoint;

	normalizedPoint.x = (point.x - x) / ((x + w) - x);
	normalizedPoint.y = (point.y - y) / ((y + h) - y);

	return normalizedPoint;
}

ImVec2 W_Scene::GetRegionAvailable()
{
	ImVec2 size;

	size.x = ImGui::GetContentRegionAvail().x;
	size.y = ImGui::GetContentRegionAvail().y;

	return size;
}


void W_Scene::DrawGuizmo()
{
	if (App->moduleEditor->GetSelectedGO())
	{
		if (!ImGuizmo::IsUsing() && ImGui::IsWindowHovered())
		{
			if (App->moduleInput->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
				operation = ImGuizmo::OPERATION::TRANSLATE;
			if (App->moduleInput->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
				operation = ImGuizmo::OPERATION::ROTATE;
			if (App->moduleInput->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
				operation = ImGuizmo::OPERATION::SCALE;
			if (App->moduleInput->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
				operation = ImGuizmo::OPERATION::BOUNDS;

			if (operation == ImGuizmo::OPERATION::SCALE && mode != ImGuizmo::MODE::LOCAL)
				mode = ImGuizmo::MODE::LOCAL;
		}

		ImVec2 cornerPos = ImGui::GetWindowPos();
		ImVec2 _size = ImGui::GetContentRegionMax();

		//The small offset is due to some error margin in the rect height
		ImGuiContext& g = *ImGui::GetCurrentContext();
		int offset = ImGui::GetFrameHeight() / 2;
		ImGuizmo::SetRect(cornerPos.x, cornerPos.y + offset, _size.x, _size.y);

		ImGuizmo::SetDrawlist();
		//ImGui::PushClipRect(ImGui::GetWindowSize(), ImGui::GetWindowSize(), true);
		C_Transform* trans = App->moduleEditor->GetSelectedGO()->transform;
		float4x4 mat = trans->globalTransform.Transposed();

		//ERROR: World mode makes rotations go byebye
		if (ImGuizmo::Manipulate(App->moduleCamera->editorCamera.ViewMatrixOpenGL().v[0], App->moduleCamera->editorCamera.ProjectionMatrixOpenGL().v[0], operation, mode, mat.ptr()) && ImGui::IsWindowHovered())
		{
			mat.Transpose();
			//mat.Decompose(trans->position, trans->rotation, trans->localScale);
			trans->SetTransformWithGlobal(mat);
			trans->updateTransform = true;
			manipulatingGuizmo = true;
		}

		else if (ImGuizmo::IsUsing() == false && manipulatingGuizmo == true)
		{
			mat.Transpose();
			App->moduleEditor->shortcutManager.PushCommand(new COMM_Transform(App->moduleEditor->GetSelectedGO()->UID, mat.ptr(), oldMat.ptr()));

			manipulatingGuizmo = false;
		}

		else if (ImGuizmo::IsUsing() == false && manipulatingGuizmo == false)
		{
			mat.Transpose();
			oldMat = mat;
		}


		//ImGui::PopClipRect();

		//if (ImGuizmo::IsUsing())
		//{
		//	float4x4 newMatrix;
		//	newMatrix.Set(modelPtr);
		//	modelProjection = newMatrix.Transposed();
		//	gameObject->GetComponent<C_Transform>()->SetGlobalTransform(modelProjection);
		//}
	}
}

#endif // !STANDALONE