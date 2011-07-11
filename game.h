// Template, major revision 3, beta
// IGAD/NHTV - Jacco Bikker - 2006-2009

#pragma once

class DummyCallback : public CollisionCallback {
	virtual bool receiveEvent(const CollisionEvent& evnt);
};

class DebugConsole;
class StatusConsole;

class PreGameState;
class LoadingState;
class InGameState;
class GameOverState;

enum OBJECT_TYPES {
	TYPE_OBSTACLE = 0,
	TYPE_PLAYER = 1,
	TYPE_MINE = 2,
	TYPE_MINE_EXPLODED = 3,
};


namespace Tmpl8 {

class Surface;
class Game {
	PreGameState* state_pg;
	InGameState* state_ig;
	GameOverState* state_go;

	float ctime;
	float ltime;
	float desired_fps;

	Surface* m_Screen;

	vec2 mousePos;

	DebugConsole* debug;
	StatusConsole* status;

	bool shiftDown;

	CInt bodies_n;

	std::list<Body*> renderQueue;

	public:
	Game() : bodies_n("bodies_n") {
		
	}
	void SetTarget( Surface* a_Surface ) { m_Screen = a_Surface; }
	void Init();
	void Tick( float a_DT );
	void KeyDown(unsigned int unicode, unsigned int scancode);
	void KeyUp(unsigned int unicode, unsigned int scancode);
	void MouseMove( unsigned int x, unsigned int y ) { mousePos.set(x, y); }
	void MouseUp( unsigned int button ) {}
	void MouseDown( unsigned int button ) {}
	Surface* getScreen() { return m_Screen; }
	DebugConsole* getDebug() { return debug; }
	StatusConsole* getStatus() { return status; }
	void addToRenderQueue(Body* b) { renderQueue.push_back(b); }
	std::list<Body*>& getRenderQueue() { return renderQueue; }
};

}; // namespace Tmpl8

class GameInstance {
	Tmpl8::Game* game;

	static GameInstance* m_instance;

	public:
	GameInstance() { game = 0; }
	~GameInstance() {}
	static GameInstance* instance() {
		if(m_instance == 0)
			m_instance = new GameInstance();
		return m_instance;
	}
	void setGame(Tmpl8::Game* g) { game = g; }
	Tmpl8::Game* getGame() { return game; }
};

class PreGameState : public GameState {
	Tmpl8::Game* game;

	public:
	PreGameState() { game = GameInstance::instance()->getGame(); }
	void init();
	void update(float frameDelta);
	void exit();
};

class InGameState : public GameState {
	Tmpl8::Game* game;

	bool firstShownFrame;
	bool firstLoadingFrame;

	public:
	InGameState() { game = GameInstance::instance()->getGame(); }
	~InGameState();
	void init();
	void update(float frameDelta);
	void exit();
};

class GameOverState : public GameState {
	Tmpl8::Game* game;

	public:
	GameOverState() { game = GameInstance::instance()->getGame(); }
	void init();
	void update(float frameDelta);
	void exit();
};