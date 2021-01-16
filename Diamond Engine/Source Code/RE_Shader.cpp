#include "RE_Shader.h"
#include"OpenGL.h"

ResourceShader::ResourceShader(unsigned int _uid) : Resource(_uid, Resource::Type::SHADER), shaderProgramID(0)
{
	for (GLuint i = 0; i < static_cast<GLuint>(Shader_OBType::SH_Max); i++)
		shaderObjects[i] = 0;


}

ResourceShader::~ResourceShader()
{
}

bool ResourceShader::LoadToMemory()
{
	return false;
}

bool ResourceShader::UnloadFromMemory()
{
	for (size_t i = 0; i < static_cast<size_t>(Shader_OBType::SH_Max); i++)
	{
		glDeleteShader(shaderObjects[i]);
		shaderObjects[i] = 0;
	}

	glDeleteShader(shaderProgramID);
	shaderProgramID = 0;

	return true;
}
