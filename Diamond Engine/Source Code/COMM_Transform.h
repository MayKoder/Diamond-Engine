#ifndef STANDALONE
#ifndef __COMM_TRANSFORM_H__
#define __COMM_TRANSFORM_H__

#include "Command.h"

class COMM_Transform : public Command
{
public:
	COMM_Transform(int agentUid, float* nextMatrix, float* previousMatrix);
	~COMM_Transform() override;

	void Execute() override;
	void Undo() override;

private:
	float nextMatrix[16];
	float previousMatrix[16];
};

#endif // !__COMM_TRANSFORM_H__
#endif // !STANDALONE
