#ifndef __W_CONSOLE_H__
#define __W_CONSOLE_H__

#include "Window.h"
#include <vector>

enum class LogType;

struct LogMessage
{
	LogMessage(std::string, LogType);
	bool EqualsStr(const char*);

	std::string msg;
	LogType lType;
	unsigned int prints;
};

class W_Console : public Window
{

public:
	W_Console();
	virtual ~W_Console();

	void Draw() override;
	void AddLog(const char*, LogType);

	char GetMsgType(LogType, ImVec4&);

	std::vector<LogMessage> logs;
	bool collapseMode;

private:
	bool scrollToBottom;
	
	//TODO: This should be a texture
	char logTypeChar;

};

#endif //__W_CONSOLE_H__

