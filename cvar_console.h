#ifndef __CVAR_CONSOLE_H
#define __CVAR_CONSOLE_H

#define STR_BUFFER 128
#define ARG_BUFFER 16

class CVarConsole;

class CFuncDef {
	public:
	std::list<char*> exports;
	virtual bool Call(char* func, CVarConsole* console, char args[ARG_BUFFER][STR_BUFFER], int args_n) = 0; 
};

inline char* strip(char* str) {
	if(strlen(str) == 0) return str;
	char* newptr = str; 
	for(int x=0; x<strlen(str); x++) {
		if(isspace(str[x])) newptr++;
		else break;
	}
	for(int x=strlen(str)-1; x>=0; x++) {
		if(isspace(str[x])) str[x] = 0;
		else break;
	}
	return newptr;
}

inline char* stripVar(char* input) {
	int len = strlen(input);

	if(input[0] == '$' && input[1] == '{' && input[len-1] == '}') {
		input[len-1] = 0;
		input++;
		input++;
	} else if(input[0] == '$') {
		input++;
	} else {
		return NULL;
	}

	return input;
}

class CVarConsole {
	std::list<std::string> out_buffer;
	CFuncDef* funcDef;

	public:
	CVarConsole();
	~CVarConsole();
	bool parseInput(char* input);
	bool parseCommand(char* input);
	bool parseAssignment(char* input);
	bool parseIdentifier(char* input);
	std::string outputNextLine();
	bool hasOutput();
	void print(char* str);
	void passFuncDefClass(CFuncDef* func);
};

#endif