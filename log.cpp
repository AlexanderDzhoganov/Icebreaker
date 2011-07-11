#pragma warning(disable: 4530)

#include <string>
#include <fstream>
#include <iostream>
#include <list>

#include "log.h"
#include "log_file.h"
#include "log_stdout.h"

Log* Log::m_instance = 0;

void LogInterfaceFactory::createFileLog(std::string filename) {
	LogFile* log = new LogFile(filename);
	Log::instance()->addInterface(log);
}

void LogInterfaceFactory::createStdoutLog() {
	LogStdout* log = new LogStdout();
	Log::instance()->addInterface(log);
}