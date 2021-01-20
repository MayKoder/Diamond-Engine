#include "DE_Cubemap.h"
#include"OpenGL.h"
#include"RE_Shader.h"
#include"CO_Camera.h"
#include"MathGeoLib/include/Math/float4x4.h"

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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DE_Cubemap::ClearMemory()
{
	if (textureID != 0)
		glDeleteTextures(1, &textureID);

	if (vboId != 0)
		glDeleteBuffers(1, &vboId);

	if(shaderRes)
		shaderRes->UnloadFromMemory();
}

void DE_Cubemap::DrawAsSkybox(C_Camera* _camera)
{
	shaderRes->Bind();


	GLint modelLoc = glGetUniformLocation(shaderRes->shaderProgramID, "view");
	float4x4 test = _camera->ViewMatrixOpenGL().Float3x3Part();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, test.ptr());

	modelLoc = glGetUniformLocation(shaderRes->shaderProgramID, "projection");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, _camera->ProjectionMatrixOpenGL().ptr());

	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	shaderRes->Unbind();

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindVertexArray(0);
}
