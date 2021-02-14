#ifndef STANDALONE
#ifndef __COMM_TRANSLATE_H__
#define __COMM_TRANSLATE_H__

#include "Command.h"

class COMM_Translate : public Command
{
public:
	COMM_Translate(GameObject* agent, float* posChange);
	~COMM_Translate() override;

	void Execute() override;
	void Undo() override;

private:
	float positionChange[3];
};

#endif // !__COMM_TRANSLATE_H__
#endif // !STANDALONE
