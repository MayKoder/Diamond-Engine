#pragma once

#include "Window.h"
#include"ImTextEditor/TextEditor.h"

class Application;
class W_TextEditor : public Window
{

public:
	W_TextEditor();
	virtual ~W_TextEditor();

	void Draw() override;

	void SetTextFromFile(const char* path);

private:
	TextEditor txtEditor;
	const char* txtName;
};


