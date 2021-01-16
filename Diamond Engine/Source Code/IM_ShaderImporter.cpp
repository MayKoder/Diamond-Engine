#include "IM_ShaderImporter.h"
#include "OpenGL.h"
#include<string>
#include"Globals.h"

void ShaderImporter::Import(char* buffer, int bSize, Resource* res)
{
	//Load/Compile vertex binary and frag binary to resource shaderObjects array
	//Create shaderProgram

	//Save .vert and .frag to .shdr? Like meshes
	//Save shaderProgram as .shdr to library
		




	//GLuint vertexShader;
	//vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	//glCompileShader(vertexShader);
	//GLint success;
	//GLchar infoLog[512];
	//glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	//if (success == 0)
	//{
	//	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
	//	LOG("Shader compilation error: %s", infoLog);
	//	glDeleteShader(vertexShader);
	//}
}

int ShaderImporter::GetAssetsShaderType(const char* assetsPath)
{
	std::string ext(assetsPath);
	ext = ext.substr(ext.find_last_of('.') + 1);

	for (int i = 0; i < ext.length(); i++)
	{
		ext[i] = std::tolower(ext[i]);
	}

	if (ext == "vert")
		return GL_VERTEX_SHADER;	
	
	if (ext == "frag")
		return GL_FRAGMENT_SHADER;

	LOG(LogType::L_ERROR, "SHADER EXTENSION NOT FOUND");
	return 0;
}
