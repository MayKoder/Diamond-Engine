#ifndef __W_CONSOLE_H__
#define __W_CONSOLE_H__

#include "Window.h"
#include <vector>

enum class LogType
{
	L_NORMAL, L_WARNING, L_ERROR
};

struct LogMessage
{
	LogMessage(std::string s_msg);
	bool EqualsStr(const char* cmp);

	std::string msg;
	unsigned int prints;
};

class W_Console : public Window
{

public:
	W_Console();
	virtual ~W_Console();

	void Draw() override;
	void AddLog(std::string s_msg);

	std::vector<LogMessage> logs;
	bool collapseMode;


};

#endif //__W_CONSOLE_H__

