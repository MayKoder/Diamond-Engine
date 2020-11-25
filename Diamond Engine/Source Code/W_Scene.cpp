#include "W_Scene.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"

#include "OpenGL.h"
#include"Application.h"
#include"M_Editor.h"

#include"ImGui/imgui_internal.h"

#include"C_Transform.h"
#include"ModuleInput.h"
#include"GameObject.h"

#include"I_FileSystem.h"

#include"MathGeoLib/include/Math/float4x4.h"
#include"MathGeoLib/include/Geometry/LineSegment.h"

W_Scene::W_Scene(Application* _app) : Window() /*: texColorBuffer(-1)*/
{
	name = "Scene";
	App = _app;

	operation = ImGuizmo::OPERATION::TRANSLATE;
	mode = ImGuizmo::MODE::WORLD;
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
		if (/*ImGui::IsWindowFocused() &&*/ ImGui::IsWindowHovered()) 
		{
			//TODO: Uncomment this, it's the editor camera input handler
			App->moduleCamera->ProcessSceneKeyboard();
		}

		ImVec2 size = ImGui::GetContentRegionAvail();

		App->moduleCamera->editorCamera.SetAspectRatio(ImGui::GetContentRegionAvail().x / ImGui::GetContentRegionAvail().y);
		ImGui::Image((ImTextureID)App->moduleCamera->editorCamera.texColorBuffer, size, ImVec2(0, 1), ImVec2(1, 0));

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_ASSET"))
			{
				//Drop asset from Asset window to scene window
				const char* name = (const char*)payload->Data;
				
				FileSystem::LoadDroppedFile(name);
				LOG(LogType::L_WARNING, "File %s loaded to scene", name);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SetCursorPos(ImVec2(10, 30));
		if (ImGui::Button((mode == ImGuizmo::MODE::LOCAL) ? "LOCAL" : "WORLD"))
			(mode == ImGuizmo::MODE::LOCAL) ? mode = ImGuizmo::MODE::WORLD : mode = ImGuizmo::MODE::LOCAL;

		//Draw gizmo
		if (EngineExternal->moduleEditor->GetSelectedGO())
		{
			if (!ImGuizmo::IsUsing()) {
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
			if (ImGuizmo::Manipulate(App->moduleCamera->editorCamera.GetOpenGLViewMatrix().v[0], App->moduleCamera->editorCamera.GetOpenGLProjectionMatrix().v[0], operation, mode, mat.ptr()))
			{
				mat.Transpose();
				//mat.Decompose(trans->position, trans->rotation, trans->localScale);
				trans->SetTransformWithGlobal(mat);
				trans->updateTransform = true;
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

		if (ImGui::IsMouseClicked(0) && !ImGuizmo::IsUsing())
		{
			ImVec2 position = ImGui::GetMousePos();
			ImVec2 normal = NormalizeOnWindow(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetFrameHeight(), ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - ImGui::GetFrameHeight(), position);
			normal.x = (normal.x - 0.5f) / 0.5f;
			normal.y = -((normal.y - 0.5f) / 0.5f);

			if ((normal.x >= -1 && normal.x <= 1) && (normal.y >= -1 && normal.y <= 1))
			{
				LineSegment picking = App->moduleCamera->editorCamera.camFrustrum.UnProjectLineSegment(normal.x, normal.y);
				//ImGui::SetCursorPos(ImVec2(100, 30));
				//ImGui::Text("%f, %f", normal.x, normal.y);
				App->moduleRenderer3D->RayToMeshQueueIntersection(picking);
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
