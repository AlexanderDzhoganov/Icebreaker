#ifndef __LOG_H
#define __LOG_H

#define LOG(x) Log::instance()->logMsg(x)

class LogInterface {
	public:
	virtual void logMsg(std::string msg) = 0;
};

class Log {
	static Log* m_instance;

	std::list<LogInterface*> ifaces;

	public:
	Log() {}
	~Log() {
		for(std::list<LogInterface*>::iterator x = ifaces.begin(); x != ifaces.end(); x++) {
			delete *x;
		}
	}
	void logMsg(std::string msg) {
		for(std::list<LogInterface*>::iterator x = ifaces.begin(); x != ifaces.end(); x++) {
			(*x)->logMsg(msg);
		}
	}
	static Log* instance() {
		if(m_instance == 0) {
			m_instance = new Log();
		}
		return m_instance;
	}
	void addInterface(LogInterface* log_iface) {
		ifaces.push_back(log_iface);
	}
};

class LogInterfaceFactory {
	public:
	static void createFileLog(std::string filename);
	static void createStdoutLog();
	static void createConsoleLog();
};

#endif