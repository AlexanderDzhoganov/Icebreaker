#ifndef __STATE_MANAGER_H
#define __STATE_MANAGER_H

class GameState {
	public:
	virtual void init() = 0;
	virtual void update(float frameDelta) = 0;
	virtual void exit() = 0;
};

class StateManager {
	static StateManager* m_instance;
	
	boost::unordered_map<std::string, GameState*> states;
	std::string currentStateName;
	GameState* currentState;

	bool firstState;

	public:
	StateManager();
	~StateManager();
	void update(float frameDelta);
	void registerState(std::string name, GameState* state);
	void transition(std::string new_state);
	static StateManager* instance() {
		if(m_instance == 0)
			m_instance = new StateManager();
		return m_instance;
	}
};

#endif