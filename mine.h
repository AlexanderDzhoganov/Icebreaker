#ifndef __MINE_H
#define __MINE_H

class Mine;

class MineCollisionCallback : public CollisionCallback {
	Mine* mine;

	public:
	MineCollisionCallback(Mine* m) : mine(m) {}
	virtual bool receiveEvent(const CollisionEvent& evnt);
};

class Mine : public SceneNode {
	MineCollisionCallback ccb;

	public:
	Body* body;
	Mine(const vec2& pos);
	~Mine();
	void update(float frameDelta);
};

#endif