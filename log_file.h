#ifndef __LOG_FILE
#define __LOG_FILE

class LogFile : public LogInterface {
	std::ofstream file;
	
	public:
	LogFile(std::string filename) {
		file.open(filename.c_str(), std::fstream::out);
	}
	~LogFile() {
		file.close();
	}
	virtual void logMsg(std::string msg) {
		file.write(msg.c_str(), msg.length()*sizeof(char));
		file.write("\n", sizeof(char));
		file.flush();
	}
};

#endif