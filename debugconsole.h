#ifndef __DEBUGCONSOLE_H
#define __DEBUGCONSOLE_H

class DebugConsole;

class FuncDef : public CFuncDef {
	DebugConsole* debug;

	public:
	FuncDef(DebugConsole* debug_n) {
		debug = debug_n;
		exports.push_back("print");
		exports.push_back("print_s");
		exports.push_back("clear");
		exports.push_back("draw");
		exports.push_back("dynlist");
		exports.push_back("init");
	}
	~FuncDef() {}
	bool Call(char* func, CVarConsole* console, char args[ARG_BUFFER][STR_BUFFER], int args_n);
};

class DebugConsole : public SceneNode {
	CVarConsole* console;
	FuncDef* fdef;

	std::list<char*> history;	
	int max_lines_num;
	int history_pos;

	float pos_x, pos_y, size_x;

	char buffered_input[STR_BUFFER];
	int buffered_input_ptr;

	std::list<char*> vectorsToDraw;

	bool isVisible;

	bool dynList;
	float dynListTimer;
	float dynListUpdateInterval;

	Tmpl8::Game* game;

	public:
	DebugConsole(Tmpl8::Game* game_n, int max_lines_num_n, float pos_x_n, float pos_y_n, float size_x_n);
	~DebugConsole();
	void getKey(char key);
	void addMsg(char* text);
	void update(float frameDelta);
	void drawSelf(Tmpl8::Surface* screen);
	bool checkCollision(float test_x, float test_y);
	void offsetHistory(int offset);
	void clear();
	void addVectorToDraw(char* vec_name);
	void clearVectorsToDraw();
	void drawVector(const vec2& v);
	void setDynList(bool state);
	void setDynListUpdateInterval(float interval);
	void statusPrint(char* msg);
};

class StatusConsole : public SceneNode {
	std::list<char*> history;	
	int max_lines_num;
	int history_pos;

	Tmpl8::Game* game;

	float pos_x, pos_y, size_x;

	public:
	StatusConsole(Tmpl8::Game* game_n, int max_lines_num_n, float pos_x_n, float pos_y_n, float size_x_n);
	~StatusConsole();
	void set(int max_lines_num_n, float pos_x_n, float pos_y_n, float size_x_n);
	void addMsg(char* text);
	void clear();
	void update(float frameDelta);
	void drawSelf(Tmpl8::Surface* screen);
};

#endif