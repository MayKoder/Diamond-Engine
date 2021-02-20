#ifndef STANDALONE
#ifndef __COMM_DELETE_GO_H__
#define __COMM_DELETE_GO_H__

#include "Command.h"

#include"DEJsonSupport.h"

class GameObject;

class COMM_DeleteGO : public Command
{
public:
	COMM_DeleteGO(GameObject* agent);
	~COMM_DeleteGO() override;

	void Execute() override;
	void Undo() override;

private:
	JSON_Value* value = nullptr;
	std::string agentName;

	int copyParentUid;
};


#endif // !__COMM_DELETE_GO_H__
#endif // !STANDALONE
