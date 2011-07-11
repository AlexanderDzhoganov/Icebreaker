#ifndef __SCENE_MANAGER_H
#define __SCENE_MANAGER_H

class SceneRoot : public SceneNode {
	public:
	SceneRoot(SceneNode* parent_) : SceneNode(parent_) {}
	~SceneRoot() {}
	void update(float frameDelta) {
		SceneNode::update(frameDelta);
	}
};

class SceneManager {
	boost::unordered_map<int, SceneNode*> ids;
	boost::unordered_map<std::string, SceneNode*> names;

	int nodes_n;
	int nodes_ids;
	SceneNode* root;

	static SceneManager* m_instance;

	int getNextId();

	public:
	SceneManager();
	~SceneManager();
	void update(float frameDelta);
	void addNode(SceneNode* node);
	void delNode(SceneNode* node);
	SceneNode* getRoot();
	SceneNode* getNodeById(int id);
	void setName(std::string name, SceneNode* node);
	SceneNode* getNodeByName(std::string name);
	static SceneManager* instance() {
		if(!m_instance)
			m_instance = new SceneManager();
		return m_instance;
	}
	bool doesNodeExist(int id);
};


#endif