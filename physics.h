#ifndef __PHYSICS_H
#define __PHYSICS_H

struct IntegratorState {
	bool initialized;
	bool sleep;
	std::vector<vec2> v_old;

	IntegratorState() {
		initialized = false;
	}
	~IntegratorState() {}
};

struct BoundingCircle {
	vec2 pos;
	float radius;
};

struct Ray {
	vec2 pos;
	vec2 dir;
	Ray(const vec2& p, const vec2& d) : pos(p), dir(d) {}
};

float randOneZero();
bool findBestSplit(Shape& shape, const vec2& hit_p, int hit_edgeA, int hit_edgeB, vec2& ret_end_p);
__forceinline bool pointLineSide(const vec2& l_start, const vec2& l_end, const vec2& p) {
	return ((l_end[0] - l_start[0])*(p[1] - l_start[1]) - (l_end[1] - l_start[1])*(p[0] - l_start[0])) > 0;
}
bool intersectSepAxis(Shape& shapeA, Shape& shapeB, vec2& normal, float& dist, bool& edgeBelongsToA, int& edge1, int& edge2, int& vert);


struct CollisionEvent;

class CollisionCallback {
	public:
	virtual bool receiveEvent(const CollisionEvent& evnt) = 0;
};

extern vec2 angleSortPredicate_center;
bool angleSortPredicate(const vec2& v1, const vec2& v2);
vec2 averageSet(const std::list<vec2>& set);
float getSetArea(const std::list<vec2>& set);
float getSetArea(const std::vector<vec2>& set);

class Body {
	Shape shape;
	BoundingCircle bcircle;
	IntegratorState state;

	CollisionCallback* callback;
	int timesSplit;
	bool breakable;
	float breakTimeout;
	CInt debug;
	int outward_vertex;

	bool simulate;

	float damping;
	int type;

	float bcircle_cache_time;

	float mass;

	public:
	bool deleteWhenPossible;
	bool released;

	int change_type;
	float change_type_delay;

	Body();
	~Body();
	void update(float frameDelta);
	Body* split(const vec2& cut_a, const vec2& cut_b);
	void fromShape(const Shape& s);
	void fromConvexSet(std::list<vec2>& point_set);
	void calculateBounds();
	Shape& getShape() { return shape; }
	BoundingCircle& getBoundingCircle();
	IntegratorState& getIntegratorState() { return state; }
	void setCollisionCallback(CollisionCallback* cb) { callback = cb; }
	CollisionCallback* getCollisionCallback() { return callback; }
	void setBreakable(bool s) { breakable = s; }
	bool getBreakable() { return breakable; }
	void setTimesSplit(int t) { timesSplit = t; }
	bool doSimulate() { return simulate; }
	void setDamping(float d) { damping = d; }
	float getDamping() { return damping; }
	int getType() { return type; }
	void setType(int t) { type = t; }
	float getMass() { return mass; }
	void setMass(float m) { mass = m; }
	void setChangeTypeDelay(int type, float delay) { change_type = type, change_type_delay = delay; }
};

struct CollisionEvent {
	bool intersected;
	Body* self;
	Body* intersected_with;
	vec2 normal;
	float dist;
	bool vertexOrEdge;
	int vertex;
	int edge1, edge2;

	void setIntersected(Body* s, Body* b, vec2& nrm, float distance, bool edgeIsA, int e1, int e2, int vert) {
		self = s;
		intersected_with = b;
		normal = nrm;
		dist = distance;
		edge1 = e1;
		edge2 = e2;
		vertex = vert;
		vertexOrEdge = edgeIsA;
	}
};

class ConstraintSolver {
	static ConstraintSolver* m_instance;

	int iterations;

	public:
	ConstraintSolver() { iterations = 8; }
	~ConstraintSolver() {}
	void setIterations(int i) { iterations = i; }
	void doBody(Body* body);
	static ConstraintSolver* instance() {
		if(m_instance == 0)
			m_instance = new ConstraintSolver();
		return m_instance;
	}
};

class VerletIntegrator {
	static VerletIntegrator* m_instance;

	float deltaT_old;

	public:
	VerletIntegrator() { deltaT_old = 1.f; }
	~VerletIntegrator() {}
	void initBody(Body* body);
	void doBody(float deltaT, Body* body);
	static VerletIntegrator* instance() {
		if(m_instance == 0)
			m_instance = new VerletIntegrator();
		return m_instance;
	}
};

bool rayLineSegmentIntersection(const Ray& ray, const vec2& seg_start, const vec2& seg_end, vec2& ret_intersection);
__forceinline bool intersectBoundingCircle(Body* bodyA, Body* bodyB) {
	BoundingCircle& a = bodyA->getBoundingCircle();
	BoundingCircle& b = bodyB->getBoundingCircle();

	if((a.pos - b.pos).len2() >= a.radius*a.radius+2*a.radius*b.radius+b.radius*b.radius) return false;

	return true;
}
bool intersectSepAxis();

class PhysicsManager {
	static PhysicsManager* m_instance;
	
	float m_world_size;

	std::list<Body*> bodies_pool;

	std::set<Body*> bodies;

	float time;
	int calls;
	bool performanceWarning;
	bool preparation;

	public:
	PhysicsManager();
	~PhysicsManager();
	void registerBody(Body* body);
	void unregisterBody(Body* body);
	void advanceSimulation(float deltaT);
	void resolveCollisions(Body* body);

	Body* getFromPool();
	void releaseToPool(Body* body);
	float getTime() { return time; }
	static PhysicsManager* instance() {
		if(m_instance == 0)
			m_instance = new PhysicsManager();
		return m_instance;
	}
	bool isPreparation() { return preparation; }
};

#endif