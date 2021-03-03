#include "DE_Cubemap.h"
#include"OpenGL.h"
#include"RE_Shader.h"
#include"CO_Camera.h"
#include"MathGeoLib/include/Math/float4x4.h"
#include"Application.h"
#include"MO_ResourceManager.h"

DE_Cubemap::DE_Cubemap() : shaderRes(nullptr), textureID(0), vboId(0)
{
}

DE_Cubemap::~DE_Cubemap()
{
}

void DE_Cubemap::CreateGLData()
{
	glGenBuffers(1, &vboId);

	// bind VBO in order to use
	glBindBuffer(GL_ARRAY_BUFFER, vboId);

	// upload data to VBO
	glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), skyboxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DE_Cubemap::ClearMemory()
{
	if (textureID != 0)
		glDeleteTextures(1, &textureID);

	if (vboId != 0)
		glDeleteBuffers(1, &vboId);

	if (shaderRes)
		EngineExternal->moduleResources->UnloadResource(shaderRes->GetUID());
}

void DE_Cubemap::DrawAsSkybox(C_Camera* _camera)
{
	glDepthFunc(GL_LEQUAL);
	//glDisable(GL_DEPTH_TEST);
	//glDepthFunc(GL_EQUAL);

	//glDepthMask(GL_FALSE);
	//glDepthRange(1.f, 1.f);
	//glEnable(GL_DEPTH_CLAMP);

	shaderRes->Bind();

	bool cameraNeedsChange = false;
	FrustumType cameraType = _camera->camFrustrum.type;

	if(cameraType != FrustumType::PerspectiveFrustum)
	{ 
		cameraNeedsChange = true;
		_camera->camFrustrum.type = FrustumType::PerspectiveFrustum;
		_camera->camFrustrum.verticalFov = 60.0 * DEGTORAD;
		_camera->camFrustrum.horizontalFov = 2.0f * atanf(tanf(_camera->camFrustrum.verticalFov / 2.0f) * 1.7f);
	}

	GLint modelLoc = glGetUniformLocation(shaderRes->shaderProgramID, "view");
	float4x4 test = _camera->ViewMatrixOpenGL().Float3x3Part();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, test.ptr());

	modelLoc = glGetUniformLocation(shaderRes->shaderProgramID, "projection");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, _camera->ProjectionMatrixOpenGL().ptr());

	//glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glDrawArrays(GL_TRIANGLES, 0, 36);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindVertexArray(0);

	//glDepthRange(0.f, 1.f);
	glDepthFunc(GL_LESS);
	//glDisable(GL_DEPTH_CLAMP);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);

	if(cameraNeedsChange)
		_camera->camFrustrum.type = FrustumType::OrthographicFrustum;

	shaderRes->Unbind();
}
