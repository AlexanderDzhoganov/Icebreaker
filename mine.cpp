#include "common.h"
#include "mine.h"

bool MineCollisionCallback::receiveEvent(const CollisionEvent& evnt) {
	int i_type = evnt.intersected_with->getType();
	if(PhysicsManager::instance()->isPreparation()) return false;
	if(evnt.self->getType() != TYPE_MINE) return false;
	if(i_type != TYPE_PLAYER && i_type != TYPE_MINE_EXPLODED) return false;

	Body* b[5];
	b[0] = evnt.self;
	for(int i=1; i<5; i++)
		b[i] = PhysicsManager::instance()->getFromPool();

	vec2 center = evnt.self->getShape().getCenter();

	std::vector<vec2>& v = evnt.self->getShape().getVertices();
	std::vector<vec2> normals = evnt.self->getShape().buildNormals();
	std::list<vec2> psets[5];

	for(int i=0; i<5; i++) {
		vec2 v1 = center;
		int v2i, v3i;
		if(i==4) {
			v2i = 0;
			v3i = 4;
		} else {
			v2i = i;
			v3i = i+1;
		}
		vec2 v2 = v[v2i];
		vec2 v3 = v[v3i];
		psets[i].push_back(v1);
		psets[i].push_back(v2);
		psets[i].push_back(v3);
	}

	for(int i=0; i<5; i++) { 
		Body* body = b[i];
		body->fromConvexSet(psets[i]);
		body->setCollisionCallback(0);
		body->getShape().setColor(0xFF0000);
		body->setMass(5.f);
		body->setType(TYPE_MINE_EXPLODED);
		body->setChangeTypeDelay(TYPE_OBSTACLE, 1000.f);
		std::vector<vec2>& v_old = body->getIntegratorState().v_old;

		for(int q=0; q<3; q++)
			v_old[q] += normals[i]*12.f;

		if(i != 0)
			GameInstance::instance()->getGame()->addToRenderQueue(body);
	}

	

	SceneManager::instance()->delNode(mine);

	return true;
}

Mine::Mine(const vec2& pos) : SceneNode(SceneManager::instance()->getRoot()), ccb(this) {
	std::list<vec2> ps;

	float a = 0.f;
	float scale = 30.f;
	for(int i=0; i<5; i++) {
		vec2 v(cosf(a), sinf(a));
		ps.push_back(pos + v*scale);
		a += 1.256628f;
	}

	body = PhysicsManager::instance()->getFromPool();
	body->fromConvexSet(ps);
	body->setBreakable(false);
	//body->setDamping(0.8f);
	body->setCollisionCallback(&ccb);
	body->setType(TYPE_MINE);
}

Mine::~Mine() {

}

void Mine::update(float frameDelta) {
	if(!body->doSimulate() || PhysicsManager::instance()->getTime() < 5000.f) return;
	Rasterizer::instance()->doShape(body->getShape(), 0xFF0000);
}