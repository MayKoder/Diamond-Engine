#include "CO_DirectionalLight.h"
#include "OpenGL.h"
#include "Application.h"
#include"MO_Renderer3D.h"

#include"GameObject.h"
#include"CO_Transform.h"
#include"CO_Camera.h"
#include"ImGui/imgui.h"
#include"MO_ResourceManager.h"
#include"RE_Shader.h"

#include"RE_Material.h"
#include"RE_Shader.h"
#include"MO_Window.h"

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
C_DirectionalLight::C_DirectionalLight(GameObject* _gm) : Component(_gm)
{
	name = "Directional Light";

	orthoFrustum.type = FrustumType::OrthographicFrustum;
	orthoFrustum.nearPlaneDistance = 0.1f;
	orthoFrustum.farPlaneDistance = 500.f;
	orthoFrustum.front = gameObject->transform->GetForward();
	orthoFrustum.up = gameObject->transform->GetUp();
	orthoFrustum.pos = gameObject->transform->position;
	orthoFrustum.orthographicWidth = 100;
	orthoFrustum.orthographicHeight = 100;

	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	depthShader = dynamic_cast<ResourceShader*>(EngineExternal->moduleResources->RequestResource(248150058, Resource::Type::SHADER));
	EngineExternal->moduleRenderer3D->directLight = this;
}

C_DirectionalLight::~C_DirectionalLight()
{
	glDeleteBuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);
	EngineExternal->moduleResources->UnloadResource(depthShader->GetUID());
	EngineExternal->moduleRenderer3D->directLight = nullptr;
}

void C_DirectionalLight::Update()
{
	//Maybe dont update every frame?
	orthoFrustum.pos = gameObject->transform->globalTransform.TranslatePart();
	orthoFrustum.front = gameObject->transform->GetForward();
	orthoFrustum.up = gameObject->transform->GetUp();

	C_Camera::LookAt(orthoFrustum, float3::zero);

#ifndef STANDALONE
	float3 points[8];
	orthoFrustum.GetCornerPoints(points);

	ModuleRenderer3D::DrawBox(points, float3(0.180f, 1.f, 0.937f));
#endif // !STANDALONE

	spaceMatrixOpenGL = (orthoFrustum.ProjectionMatrix() * orthoFrustum.ViewMatrix()).Transposed();
}

#ifndef STANDALONE
bool C_DirectionalLight::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Image((ImTextureID)depthMap, ImVec2(250, 250), ImVec2(0, 1), ImVec2(1, 0));

		if (ImGui::DragFloat2("Ortho size", orthoSize.ptr(), 0.001f)) 
		{
			orthoFrustum.orthographicWidth = SHADOW_WIDTH / orthoSize.x;
			orthoFrustum.orthographicHeight = SHADOW_HEIGHT / orthoSize.y;
		}

		return true;
	}
	return false;
}
#endif // !STANDALONE

void C_DirectionalLight::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

}
void C_DirectionalLight::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);


}

void C_DirectionalLight::StartPass()
{

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glDisable(GL_CULL_FACE);

	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// 1. first render to depth map
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//glLoadIdentity();
	//glMatrixMode(GL_PROJECTION);
	//glLoadMatrixf((GLfloat*)orthoFrustum.ProjectionMatrix().Transposed().v);

	//math::float4x4 mat = orthoFrustum.ViewMatrix();
	//mat.Transposed();
	//glMatrixMode(GL_MODELVIEW);
	//glLoadMatrixf((GLfloat*)mat.v);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	//glClearColor(0.08f, 0.08f, 0.08f, 1.f);
	glClear(GL_DEPTH_BUFFER_BIT);

	depthShader->Bind();
}

void C_DirectionalLight::PushLightUniforms(ResourceMaterial* material)
{
	GLint modelLoc = glGetUniformLocation(material->shader->shaderProgramID, "lightSpaceMatrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, this->spaceMatrixOpenGL.ptr());

	modelLoc = glGetUniformLocation(material->shader->shaderProgramID, "lightPos");
	glUniform3fv(modelLoc, 1, &gameObject->transform->position.x);

	modelLoc = glGetUniformLocation(material->shader->shaderProgramID, "viewPos");
	glUniform3fv(modelLoc, 1, EngineExternal->moduleRenderer3D->activeRenderCamera->GetPosition().ptr());

	//glUniform1i(glGetUniformLocation(material->shader->shaderProgramID, shadowMap), used_textures);

	glActiveTexture(GL_TEXTURE0 + 5);
	modelLoc = glGetUniformLocation(material->shader->shaderProgramID, "shadowMap");
	glUniform1i(modelLoc, 5);
	glBindTexture(GL_TEXTURE_2D, depthMap);
}

void C_DirectionalLight::EndPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, 0);

	//glBindTexture(GL_TEXTURE_2D, depthMap);

	depthShader->Unbind();

	glEnable(GL_CULL_FACE);
	glViewport(0, 0, EngineExternal->moduleWindow->s_width, EngineExternal->moduleWindow->s_height);
}

//#version 330 core
//layout(location = 0) in vec3 aPos;
//
//uniform mat4 lightSpaceMatrix;
//uniform mat4 model;
//
//void main()
//{
//	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
//}

//#version 330 core
//
//void main()
//{
//	// gl_FragDepth = gl_FragCoord.z;
//}