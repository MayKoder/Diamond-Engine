#ifndef STANDALONE

#ifndef __W_CONSOLE_H__
#define __W_CONSOLE_H__

#include "Window.h"
#include <vector>

enum class LogType;

struct LogMessage
{
	LogMessage(std::string&, LogType);
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
	void FilterLogs();

	void ClearLogs();
	ImTextureID GetMsgType(LogType);

	std::vector<LogMessage> logs;
	std::vector<LogMessage> visibleLogs;
	bool collapseMode;

private:
	bool scrollToBottom;
	bool showMessage[3];

	//TODO: This should be a texture
	char logTypeChar;

};

#endif //__W_CONSOLE_H__

#endif // !STANDALONE