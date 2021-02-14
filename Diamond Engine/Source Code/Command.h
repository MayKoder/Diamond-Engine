#ifndef STANDALONE

#ifndef __COMMAND_H__
#define __COMMAND_H__

class GameObject;

class Command
{
public:
	Command(GameObject* agent);
	virtual ~Command();

	virtual void Execute() = 0;
	virtual void Undo() = 0;

protected:
	GameObject* agent = nullptr;
};

#endif // !__COMMAND_H__
#endif // !STANDALONE