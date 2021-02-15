#ifndef STANDALONE
#ifndef __COMM_TRANSFORM_H__
#define __COMM_TRANSFORM_H__

#include "Command.h"

class COMM_Transform : public Command
{
public:
	COMM_Transform(GameObject* agent, float* posChange, float* rotChange, float* sclChange);
	~COMM_Transform() override;

	void Execute() override;
	void Undo() override;

private:
	float positionChange[3];
	float rotationChange[3];
	float scaleChange[3];
};

#endif // !__COMM_TRANSFORM_H__
#endif // !STANDALONE
