#ifndef STANDALONE

#pragma once

#include "Window.h"
#include"ImTextEditor/TextEditor.h"
#include"DEResource.h"

class Application;
class W_TextEditor : public Window
{

public:
	W_TextEditor();
	virtual ~W_TextEditor();

	void Draw() override;

	void SetTextFromFile(const char* path);

	std::string txtName;
private:

	void DrawShaderEditor();
	void DrawScriptEditor();

	TextEditor txtEditor;
	Resource::Type textType;
};

#endif // !STANDALONE