#ifndef STANDALONE
#ifndef __COMM_DELETE_GO_H__
#define __COMM_DELETE_GO_H__

#include "Command.h"
#include "GameObject.h"

class COMM_DeleteGO : public Command
{
public:
	COMM_DeleteGO(GameObject* agent);
	~COMM_DeleteGO() override;

	void Execute() override;
	void Undo() override;

private:
	GameObject copy;
	int copyParentUid;
};


#endif // !__COMM_DELETE_GO_H__
#endif // !STANDALONE
