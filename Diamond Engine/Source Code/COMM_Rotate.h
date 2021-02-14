#ifndef STANDALONE
#ifndef __COMM_ROTATE_H__
#define __COMM_ROTATE_H__

#include "Command.h"

class COMM_Rotate : public Command
{
public:
	COMM_Rotate(GameObject* agent, float* rotChange);
	~COMM_Rotate() override;

	void Execute() override;
	void Undo() override;

private:
	float rotationChange[3];
};

#endif // !__COMM_ROTATE_H__

#endif // !STANDALONE
