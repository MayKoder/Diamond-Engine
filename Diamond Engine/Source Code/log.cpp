#pragma once
#include "Globals.h"

//TODO: Move the GetEditorWindow() to Application.cpp?
#include "Application.h"
#include "W_Console.h"
#include "M_Editor.h"

void log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	if (Engine != nullptr && Engine->moduleEditor != nullptr)
	{
		W_Console* consoleWindow = dynamic_cast<W_Console*>(Engine->moduleEditor->GetEditorWindow(EditorWindow::CONSOLE));

		if (consoleWindow != nullptr)
			consoleWindow->AddLog(tmp_string);
	}
}