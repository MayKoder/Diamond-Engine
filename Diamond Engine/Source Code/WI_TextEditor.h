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

private:
	TextEditor txtEditor;
};


