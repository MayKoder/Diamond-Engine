#ifndef STANDALONE
#ifndef __SHORTCUT_MANAGER_H__
#define __SHORTCUT_MANAGER_H__

#define MAX_HISTORY_LENGHT 100

#include <vector>


class Command;

class ShortcutManager
{
public:
	ShortcutManager();
	~ShortcutManager();

	void HandleInput();	//Called by the module editor

	void PushCommand(Command* command);

	//MUST be called every time the scene is changed
	void DeleteCommandHistory(); 

private:
	void UndoCommand();
	void RedoCommand();

private:
	std::vector<Command*> commandHistory;
	int index = -1;
};

#endif // !__SHORTCUT_MANAGER_H__

#endif // !STANDALONE
