#include "common.h"

// scene manager

SceneManager* SceneManager::m_instance = 0;

SceneManager::SceneManager() {
	nodes_n = 1;
	nodes_ids = 1;
	root = new SceneRoot(NULL);
	//root->appendName(std::string("Root"));
}

SceneManager::~SceneManager() {
	delete root;
}

int SceneManager::getNextId() {
	nodes_ids++;
	return nodes_ids-1;
}

void SceneManager::update(float frameDelta) {
	root->update(frameDelta);
}

void SceneManager::addNode(SceneNode* node) {
	int id = getNextId();
	node->setId(id);
	ids[id] = node;
	nodes_n++;
}

void SceneManager::delNode(SceneNode* node) {
	ids.erase(node->getId());
	nodes_n--;
	names.erase(node->getName());
}

SceneNode* SceneManager::getRoot() {
	return root;
}

SceneNode* SceneManager::getNodeById(int id) {
	if(ids.count(id) == 0)
		return NULL;
	return ids[id];
}

void SceneManager::setName(std::string name, SceneNode* node) {
	if(names.count(name) > 0) {
		char buf[256];
		sprintf(buf, "Name clash occured: %s", name.c_str());
		LOG(buf);
		return;
	}
	names[name] = node;
}

SceneNode* SceneManager::getNodeByName(std::string name) {
	if(names.count(name) == 0) return NULL;
	return names[name];
}

bool SceneManager::doesNodeExist(int id) {
	bool exists = false;
	if(ids.count(id) != 0)
		exists = true;
	return exists;
}