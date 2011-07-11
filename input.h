#ifndef __INPUT_H
#define __INPUT_H

enum KEYS {
	KEY_DOWN = 90,
	KEY_LEFT = 92,
	KEY_RIGHT = 94,
	KEY_UP = 96,
	KEY_ENTER = 40,
};

class InputManager {
	static InputManager* m_instance;

	boost::unordered_map<int, bool> keymap;

	public:
	InputManager() {}
	~InputManager() {}
	static InputManager* instance() {
		if(m_instance == 0)
			m_instance = new InputManager();
		return m_instance;
	}
	void injectKeyDown(int key) {
		if(keymap.count(key) == 0)
			keymap.insert(std::pair<int, bool>(key, true));
		else
			keymap[key] = true;
	}
	void injectKeyUp(int key) {
		keymap[key] = false;
	}
	bool getKey(int key) {
		if(keymap.count(key) == 0) return false;
		return keymap[key];
	}
};

#endif