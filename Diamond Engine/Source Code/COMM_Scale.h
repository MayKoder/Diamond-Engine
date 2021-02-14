#ifndef STANDALONE
#ifndef __COMM_SCALE_H__
#define __COMM_SCALE_H__

#include "Command.h"

class COMM_Scale : public Command
{
	COMM_Scale(GameObject* agent, float* sclChange);
	~COMM_Scale() override;

	void Execute() override;
	void Undo() override;

private:
	float scaleChange[3];
};


#endif // !__COMM_SCALE_H__

#endif // !STANDALONE