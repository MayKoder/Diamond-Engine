#ifndef STANDALONE

#ifndef __COMMAND_H__
#define __COMMAND_H__

class Command
{
public:
	Command(int agentUid);
	virtual ~Command();

	virtual void Execute() = 0;
	virtual void Undo() = 0;

protected:
	int agentUid;
};

#endif // !__COMMAND_H__
#endif // !STANDALONE