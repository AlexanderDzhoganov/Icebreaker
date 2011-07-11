#include "common.h"

SceneNode::SceneNode(SceneNode* parent_n) {
	parent = parent_n;
	if(parent) {
		parent->addChild(this);
		SceneManager::instance()->addNode(this);
	} else {
		setId(0);
	}
	destroyOnNextFrame = false;
}

SceneNode::~SceneNode() {
	deleteChildren();
	SceneManager::instance()->delNode(this);
}

void SceneNode::update(float frameDelta) {
	bool deleted = true;
	while(deleted) {
		deleted = false;	
		for(std::set<SceneNode*>::const_iterator child = children.begin(); child != children.end(); child++) {
			if((*child)->destroyOnNextFrame) {
				deleteChild(*child);
				bool deleted = true;
				break;
			}
		}
	}
	for(std::set<SceneNode*>::const_iterator child = children.begin(); child != children.end(); child++) {
 		(*child)->update(frameDelta);
	}
}

void SceneNode::addChild(SceneNode* node) {
	if(!node) {
		LOG("Trying to add NULL child");
		return;
	}
	children.insert(node);
}
	
void SceneNode::deleteChildren() {
	for(std::set<SceneNode*>::const_iterator child = children.begin(); child != children.end(); child++) {
		delete *child;
	}

	children.clear();
}

void SceneNode::deleteChild(SceneNode* node) {
	children.erase(node);
	delete node;
}

std::set<SceneNode*>* SceneNode::getChildren() {
	return &children;
}

SceneNode* SceneNode::getParent() {
	return parent;
}

void SceneNode::setId(const int& id_n) {
	id = id_n;
	char buf[256];
	sprintf(buf, "%i", id);
	name = std::string(buf);
}

const int SceneNode::getId() {
	return id;
}

void SceneNode::appendName(const std::string& ext, const bool& primary_type) {
	char buf[256];
	sprintf(buf, "%s::%s", name.c_str(), ext.c_str());
	name = std::string(buf);
	if(!primary_type) {
		SceneManager::instance()->setName(name, this);
	}
}

std::string const& SceneNode::getName() {
	return name;
}