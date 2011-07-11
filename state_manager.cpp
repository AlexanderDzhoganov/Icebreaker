#include "common.h"

StateManager* StateManager::m_instance = 0;

StateManager::StateManager() {
	currentState = NULL;
	firstState = true;
} 

StateManager::~StateManager() {

}

void StateManager::update(float frameDelta) {
	if(currentState) {
		currentState->update(frameDelta);
	}
}

void StateManager::registerState(std::string name, GameState* state) {
	if(states.count(name) > 0) {
		char buf[256];
		sprintf(buf, "Error: trying to duplicate existing state %s", name.c_str());
		LOG(buf);
		return;
	}
	states[name] = state;
	if(firstState) {
		transition(name);
	}
	firstState = false;
}

void StateManager::transition(std::string new_state) {
	if(currentState) {
		currentState->exit();
	}
	if(states.count(new_state) == 0) {
		printf("Error: state not found %s", new_state.c_str());
		return;
	}
	currentState = states[new_state];
	currentState->init();
}