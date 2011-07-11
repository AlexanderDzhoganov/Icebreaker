#ifndef __LOG_STDOUT
#define __LOG_STDOUT

class LogStdout : public LogInterface {
	public:
	virtual void logMsg(std::string msg) {
		std::cout << msg.c_str() << std::endl;
	}
};

#endif