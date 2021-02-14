#include "Command.h"

Command::Command(GameObject* agent) : agent(agent)
{
}


Command::~Command()
{
	agent = nullptr;
}