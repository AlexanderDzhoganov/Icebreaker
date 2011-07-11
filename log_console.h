#ifndef __LOG_CONSOLE
#define __LOG_CONSOLE

class LogConsole : public LogInterface {
	public:
	virtual void logMsg(std::string msg) {
		InputManager::instance()->getInterface()->printToConsole(msg);
	}
};

#endif