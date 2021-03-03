#pragma once
#ifndef STANDALONE

#include "Command.h"

class COMM_Transform2D : public Command
{
public:
	COMM_Transform2D(int agentUid, float* next_parameters, float* previous_parameters);
	~COMM_Transform2D() override;

	void Execute() override;
	void Undo() override;

private:
	float next_parameters[5];////[0]&&[1] = position; [2] = rotation; [3] && [4] = size
	float previous_parameters[5];////[0]&&[1] = position; [2] = rotation; [3] && [4] = size
};

#endif // !STANDALONE
