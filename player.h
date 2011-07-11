#ifndef __PLAYER_H
#define __PLAYER_H

class Player;

class PlayerCollisionCallback : public CollisionCallback {
	Player* player;

	public:
	PlayerCollisionCallback(Player* p) : player(p) {}
	~PlayerCollisionCallback() {}
	virtual bool receiveEvent(const CollisionEvent& evnt);
};

class Player : public SceneNode {
	public:
	float controlLoss;	
	Body* body;
	PlayerCollisionCallback ccb;

	int lead_vertex;
	
	Player();
	~Player();
	void update(float frameDelta);
};

#endif