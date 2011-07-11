// Template, major revision 3
// IGAD/NHTV - Jacco Bikker - 2006-2009

#include "common.h"
#include "mine.h"

using namespace Tmpl8;

GameInstance* GameInstance::m_instance = 0;

//Shape shp;
Player* player = 0;

InputManager* InputManager::m_instance = 0;

bool DummyCallback::receiveEvent(const CollisionEvent& evnt) {
	if(evnt.vertexOrEdge) {
		std::vector<vec2>& v = evnt.self->getShape().getVertices();
		std::vector<vec2>& vHit = evnt.intersected_with->getShape().getVertices();
		
		if(evnt.edge1 >= v.size()) return false;
		if(evnt.edge2 >= v.size()) return false;
		if(evnt.vertex >= vHit.size()) return false;

		vec2 edge1 = v[evnt.edge1];
		vec2 edge2 = v[evnt.edge2];
		vec2 vertex = vHit[evnt.vertex];

		vec2 t_e1 = edge1;
		vec2 t_e2 = edge2;
		vec2 t_v = vertex;
		t_e1 -= edge1;
		t_e2 -= edge1;
		t_v -= edge1;

		vec2 axis = t_e2 - t_e1;
		float len = axis.len();
		axis.normalize();
		float d = axis.dot_product(t_v);
		
		if(d < 0.2f*len) d = 0.2f*len;
		if(d > 0.8f*len) d = 0.8f*len;

		vec2 hit_p = edge1+axis*d;
		vec2 nrm = evnt.normal;
		vec2 end_p;

		if(findBestSplit(evnt.self->getShape(), hit_p, evnt.edge1, evnt.edge2, end_p) && evnt.dist > 0.01f) {
			Rasterizer::instance()->debugLine(hit_p, end_p, 0xFF0000);
			Body* b = evnt.self->split(hit_p, end_p);
			Rasterizer::instance()->debugPoint(hit_p, 0xFFFFFF);
			Rasterizer::instance()->debugPoint(end_p, 0xFFFFFF);
			if(b) GameInstance::instance()->getGame()->addToRenderQueue(b);
			return true;
		}
	}
	return false;
}

DummyCallback dcallback;


void PreGameState::init() {
	game->getScreen()->Clear(0);
	game->getStatus()->set(15, 50, 100, 628);
	game->getStatus()->addMsg("");
	game->getStatus()->addMsg("***** Icebreaker *****");
	game->getStatus()->addMsg("PR2 assignment by:");
	game->getStatus()->addMsg("Alexander Dzhoganov - 100397 - 1GA-1PR");
	game->getStatus()->addMsg("");
	game->getStatus()->addMsg("Controls:");
	game->getStatus()->addMsg("Arrow keys");
	game->getStatus()->addMsg("");
	game->getStatus()->addMsg("Objective:");
	game->getStatus()->addMsg("Reach the right side of the map as fast as you can!");
	game->getStatus()->addMsg("Break ice blocks to clear a path.");
	game->getStatus()->addMsg("Hitting a mine will slow you down.");
	game->getStatus()->addMsg("");
	game->getStatus()->addMsg("Press  enter  to start or  esc  to exit");
}

void PreGameState::update(float frameDelta) {
	if(InputManager::instance()->getKey(KEY_ENTER)) {
		StateManager::instance()->transition("ingame");
	}
}

void PreGameState::exit() {

}


void InGameState::init() {
	firstShownFrame = false;
	firstLoadingFrame = false;
}

char msg_time[256];
char msg_distance[256];

void InGameState::update(float frameDelta) {
	if(PhysicsManager::instance()->getTime() > 5000.f) {
		if(!firstShownFrame) {
			game->getStatus()->set(5, 10, 10, 185);
		}

		firstShownFrame = true;
		if(player == 0) player = new Player();
		game->getScreen()->Clear(0x3366CC);
		
		for(std::list<Body*>::iterator b = game->getRenderQueue().begin(); b != game->getRenderQueue().end(); b++) {
			if(!(*b)->doSimulate() || (*b)->released) continue;
			Rasterizer::instance()->doShape((*b)->getShape(), (*b)->getShape().getColor());
		}

		Rasterizer::instance()->doShape(player->body->getShape(), 0xFF00);
		vec2& cameraPos = Rasterizer::instance()->getCameraPos();
		cameraPos = player->body->getShape().getCenter()*-1.f;
		cameraPos += vec2(SCRWIDTH*0.5, SCRHEIGHT*0.5);

		game->getScreen()->Line(cameraPos[0], cameraPos[1], W_WIDTH+cameraPos[0], cameraPos[1], 0xFFFFFF);
		game->getScreen()->Line(cameraPos[0], cameraPos[1], cameraPos[0], W_HEIGHT+cameraPos[1], 0xFFFFFF);
		game->getScreen()->Line(W_WIDTH+cameraPos[0], cameraPos[1], W_WIDTH+cameraPos[0], W_HEIGHT+cameraPos[1], 0xFFFFFF);
		game->getScreen()->Line(cameraPos[0], W_HEIGHT+cameraPos[1], W_WIDTH+cameraPos[0], W_HEIGHT+cameraPos[1], 0xFFFFFF);

		vec2 target(W_WIDTH, W_HEIGHT*0.5);
		vec2 c = player->body->getShape().getCenter();
		vec2 dir = target - c;
		float distanceToGoal = dir.len();
		if(distanceToGoal < 500.f) {
			StateManager::instance()->transition("gameover");
			return;
		}
		dir.normalize();
		c += cameraPos;
		game->getScreen()->Line(c[0], c[1], c[0]+dir[0]*15, c[1]+dir[1]*15, 0x153E7E);

		game->getStatus()->clear();
		
		sprintf(msg_time, "Time: %.1f", (PhysicsManager::instance()->getTime()-5000.f)/1000.f);
		sprintf(msg_distance, "Distance: %.0f", distanceToGoal);
		game->getStatus()->addMsg("***************");
		game->getStatus()->addMsg(msg_time);
		game->getStatus()->addMsg(msg_distance);
		game->getStatus()->addMsg("***************");
	} else {
		game->getScreen()->Clear(0);
		game->getStatus()->clear();
		game->getStatus()->addMsg("LOADING");
	}
	
	if(!firstLoadingFrame) {
		firstLoadingFrame = true;
		return;
	}

	SceneManager::instance()->update(frameDelta);
	PhysicsManager::instance()->advanceSimulation(frameDelta);
}

void InGameState::exit() {

}

char msg_final_time[256];

void GameOverState::init() {
	game->getScreen()->Clear(0);
	game->getStatus()->set(15, 50, 100, 628);
	game->getStatus()->addMsg("");
	game->getStatus()->addMsg("Game Over");
	sprintf(msg_final_time, "Final time: %.1f", (PhysicsManager::instance()->getTime()-5000.f)/1000.f);
	game->getStatus()->addMsg(msg_final_time);
	game->getStatus()->addMsg("");
	game->getStatus()->addMsg("enter  to restart or  esc  to exit");
}

void GameOverState::update(float frameDelta) {
	game->getScreen()->Clear(0);
	game->getStatus()->set(15, 50, 100, 628);
	game->getStatus()->addMsg("");
	game->getStatus()->addMsg("Game Over");
	sprintf(msg_final_time, "Final time: %.1f", (PhysicsManager::instance()->getTime()-5000.f)/1000.f);
	game->getStatus()->addMsg(msg_final_time);
	game->getStatus()->addMsg("");
	game->getStatus()->addMsg("press  esc  to exit");
}

void GameOverState::exit() {

}

#define M_PI 3.14157

std::list<vec2> createConvexSet(int num_vert, float scale, const vec2& transl, int recursion=0) {
	std::list<vec2> set;

	for(int i=0; i < num_vert; i++) {
		vec2 p(-1.f+2.f*randOneZero(), -1.f+2.f*randOneZero());
		p.normalize();
		p *= scale;
		p += transl;
		set.push_back(p);
	}
	
	angleSortPredicate_center = transl;
	set.sort(angleSortPredicate);

	if(getSetArea(set) < 200.f*scale && recursion < 10)
		createConvexSet(num_vert, scale, transl, recursion+1);
	return set;
}

void Game::Init()
{
	GameInstance::instance()->setGame(this);

	Body* b;

	for(int i=0; i<512; i++) {
		Mine* m = new Mine(vec2(rand()%W_WIDTH, rand()%W_HEIGHT));
	}

	for(int i=0; i<400; i++) {
		b = PhysicsManager::instance()->getFromPool();
		b->fromConvexSet(createConvexSet(4+rand()%2, 180+rand()%50, vec2(rand()%W_WIDTH, rand()%W_HEIGHT)));
		b->setCollisionCallback(&dcallback);
		b->getShape().setColor(0x74BBFB);
		float area = getSetArea(b->getShape().getVertices());
		if(area == area) {} else {
			PhysicsManager::instance()->releaseToPool(b);
			continue;
		}
		float mass = area/100000.f;
		if(mass < 1.f) mass = 1.f;
		if(mass > 3.f) mass = 3.f;
		b->setMass(mass);
		renderQueue.push_back(b);
	}

	ctime = 0;
	ltime = 0;
	desired_fps = 240.f;

	debug = new DebugConsole(this, 16, 10, 10, 400);
	status = new StatusConsole(this, 4, 500, 10, 260);
//	status->addMsg("StatusConsole v0.1");

	state_pg = new PreGameState();
	state_ig =new InGameState();
	state_go =new GameOverState();

	StateManager::instance()->registerState("pregame", state_pg);
	StateManager::instance()->registerState("ingame", state_ig);
	StateManager::instance()->registerState("gameover", state_go);

	shiftDown = false;

	ConstraintSolver::instance()->setIterations(6);

	Rasterizer::instance()->setScreen(m_Screen);
}

void Game::Tick( float a_DT)
{
	StateManager::instance()->update(a_DT);
	status->drawSelf(getScreen());
	debug->drawSelf(getScreen());
}

void Game::KeyDown(unsigned int unicode, unsigned int scancode) {
	if(scancode == 88) scancode = 40;
	InputManager::instance()->injectKeyDown(scancode);

	if(scancode == 225) shiftDown = true;
	else if(!shiftDown) {
		debug->getKey(unicode);
	} else if(shiftDown) {
		if(unicode == '4') debug->getKey('$');
		if(unicode == '3') debug->getKey('#');
		if(unicode == '-') debug->getKey('_');
	}
}

void Game::KeyUp(unsigned int unicode, unsigned int scancode) {
	InputManager::instance()->injectKeyUp(scancode);
	if(scancode == 225) shiftDown = false;
}