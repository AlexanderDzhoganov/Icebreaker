#ifndef __SCENENODE_H
#define __SCENENODE_H

class SceneManagerOutOfIds {};

class SceneNode {
	std::set<SceneNode*> children;
	SceneNode* parent;
	int id;
	std::string name;

	public:
	bool destroyOnNextFrame;

	SceneNode(SceneNode* parent_n);
	virtual ~SceneNode() = 0;
	virtual void update(float frameDelta);
	void addChild(SceneNode* node);
	void deleteChildren();
	void deleteChild(SceneNode* node);
	std::set<SceneNode*>* getChildren();
	SceneNode* getParent();
	void setId(const int& id_n);
	const int getId();
	void appendName(const std::string& ext, const bool& primary_type=false);
	std::string const& getName();
};


#endif