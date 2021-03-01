#ifndef STANDALONE

#include "MA_ShortcutManager.h"
#include "Command.h"

#include "Application.h"
#include "MO_Input.h"

#include "Globals.h"

ShortcutManager::ShortcutManager() :
	index(-1)
{
}


ShortcutManager::~ShortcutManager()
{
	DeleteCommandHistory();
}


void ShortcutManager::HandleInput()
{	
	if (EngineExternal->moduleInput->GetKey(SDL_SCANCODE_LCTRL) == KEY_STATE::KEY_REPEAT)
	{
		if (EngineExternal->moduleInput->GetKey(SDL_SCANCODE_Z) == KEY_STATE::KEY_DOWN)
			UndoCommand();

		else if (EngineExternal->moduleInput->GetKey(SDL_SCANCODE_Y) == KEY_STATE::KEY_DOWN)
			RedoCommand();

		//TODO: Move copy and paste here
	}
	
}


void ShortcutManager::PushCommand(Command* command)
{
	if (index == MAX_HISTORY_LENGHT)
	{
		delete commandHistory[0];
		commandHistory.erase(commandHistory.begin());
	}

	else if (index < (int)(commandHistory.size() - 1))
	{
		for (int i = commandHistory.size() - 1; i > index; --i)
		{
			delete commandHistory[i];
			commandHistory.erase(commandHistory.begin() + i);
		}
	}

	//check for changes size
	commandHistory.push_back(command);
	index = commandHistory.size() - 1;
}


void ShortcutManager::DeleteCommandHistory()
{
	int commandCount = commandHistory.size();

	for (int i = 0; i < commandCount; ++i)
		delete commandHistory[i];

	commandHistory.clear();
	index = -1;
}


void ShortcutManager::UndoCommand()
{
	if (index >= 0)
	{
		commandHistory[index]->Undo();
		--index;
	}

	else
		LOG(LogType::L_WARNING, "No more changes to UNDO");
}


void ShortcutManager::RedoCommand()
{
	if (index < (int)(commandHistory.size() - 1))
	{
		++index;
		commandHistory[index]->Execute();
	}

	else
		LOG(LogType::L_WARNING, "No more changes to REDO");
}

#endif // !STANDALONE