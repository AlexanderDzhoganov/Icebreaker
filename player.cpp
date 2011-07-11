#include "common.h"
#include "player.h"

bool PlayerCollisionCallback::receiveEvent(const CollisionEvent& evnt) { 
	
	if(evnt.intersected_with->getType() == TYPE_MINE_EXPLODED) {
		player->controlLoss = 450.f;
	}

	return false;
}

Player::Player() : SceneNode(SceneManager::instance()->getRoot()), ccb(this) {
//	Shape& s = body.getShape();
	std::list<vec2> ps;
	
	vec2 pos(300, 300);

	ps.push_back(pos + vec2(0, 0));
	ps.push_back(pos + vec2(-10, 10));
	ps.push_back(pos + vec2(-10, 40));
	ps.push_back(pos + vec2(10, 40));
	ps.push_back(pos + vec2(10, 10));

	body = PhysicsManager::instance()->getFromPool();
	body->fromConvexSet(ps);
	body->setBreakable(false);

	std::vector<vec2>& v = body->getShape().getVertices();
	for(int i=0; i<v.size(); i++) {
		if(v[i] == vec2(300.f, 300.f)) lead_vertex = i;
	}
	body->setDamping(0.975f);
	body->setType(TYPE_PLAYER);
	body->setCollisionCallback(&ccb);

	controlLoss = 0.f;
	
}

Player::~Player() {
	PhysicsManager::instance()->releaseToPool(body);
}

void Player::update(float frameDelta) {
	SceneNode::update(frameDelta);

	controlLoss -= frameDelta;
	if(controlLoss > 0.f) return;
	controlLoss = 0.f;

	bool move_y = true, move_x = true;

	vec2 target;
	if(InputManager::instance()->getKey(KEY_UP)) {
		target += vec2(0.f, -1.f);
	} else if(InputManager::instance()->getKey(KEY_DOWN)) {
		target += vec2(0.f, 1.f);
	} else {
		move_y = false;
	}
	
	if(InputManager::instance()->getKey(KEY_LEFT)) {
		target += vec2(-1.f, 0.f);
	} else if(InputManager::instance()->getKey(KEY_RIGHT)) {
		target += vec2(1.f, 0.f);
	} else {
		move_x = false;
	}

	if(move_x||move_y) {
		if(move_x&&move_y) target.normalize();

		body->getShape().getVertices()[lead_vertex] += target*0.1f*frameDelta;
	}
}