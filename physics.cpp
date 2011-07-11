#include "common.h"
#include "physics.h"

float triangleArea(const std::list<vec2>& vertlist) {
	std::vector<vec2> vert;
	for(std::list<vec2>::const_iterator i = vertlist.begin(); i != vertlist.end(); i++) {
		vert.push_back(*i);
	}

	vec2 u = vert[1] - vert[0];
	vec2 v = vert[2] - vert[0];
	return 0.5f * fabs(u[0]*v[1]-u[1]*v[0]);
}

float randOneZero() {
	int r = rand()%10000;
	return float(r)/float(10000);
}

bool findBestSplit(Shape& shape, const vec2& hit_p, int hit_edgeA, int hit_edgeB, vec2& ret_end_p) {
	std::vector<vec2>& v = shape.getVertices();
	int v_n = v.size();

	vec2 edge = v[hit_edgeB] - v[hit_edgeA];
	vec2 normal(-edge[1], edge[0]);
	normal.normalize();
	
	Rasterizer::instance()->debugLine(hit_p, hit_p+normal*60.f, 0xFF00FF);

	std::vector<int> c;
	vec2 center = shape.getCenter();
	//float dist = (center - hit_p).len();
	vec2 c_dir = center - hit_p;
	vec2 edgeA = v[hit_edgeA];
	vec2 edgeB = v[hit_edgeB];
	edgeA += c_dir*1.2f;
	edgeB += c_dir*1.2f;

	Rasterizer::instance()->debugLine(edgeA, edgeB, 0xFFFFFF);

	for(int i=0; i<v_n; i++) {
		if(!pointLineSide(edgeA, edgeB, v[i])) {
			c.push_back(i);
		}
	}

	int c_n = c.size();

	int f_edgeA, f_edgeB;

	if(c_n == 0) {
		return false;
	} else if(c_n == 1) {
		f_edgeA = c[0];
		
		int f_edgeBmin = f_edgeB = c[0]-1;
		if(f_edgeB < 0) f_edgeBmin = f_edgeB = v_n-1;
		float distMinOne = (v[f_edgeA] - v[f_edgeB]).len2();
		int f_edgeBmax = f_edgeB = c[0]+1;
		if(f_edgeB >= v_n) f_edgeBmax = f_edgeB = 0;
		float distPlusOne = (v[f_edgeA] - v[f_edgeB]).len2();
		f_edgeB = distMinOne < distPlusOne ? f_edgeBmin : f_edgeBmax;
	} else if(c_n == 2) {
		f_edgeA = c[0];
		f_edgeB = c[1];
	} else {
		float maxDist = -FLT_MAX;
		int maxVert = -1;
		
		for(int i=0; i<c_n; i++) {
			float d = (v[c[i]] - hit_p).len2();
			if(d > maxDist) {
				maxDist = d;
				maxVert = c[i];
			}
		}

		if(maxVert == -1) return false;

		f_edgeA = maxVert;
		
		int f_edgeBmin = f_edgeB = maxVert-1;
		if(f_edgeB < 0) f_edgeBmin = f_edgeB = v_n-1;
		float distMinOne = (v[f_edgeA] - v[f_edgeB]).len2();
		int f_edgeBmax = f_edgeB = maxVert+1;
		if(f_edgeB >= v_n) f_edgeBmax = f_edgeB = 0;
		float distPlusOne = (v[f_edgeA] - v[f_edgeB]).len2();
		f_edgeB = distMinOne < distPlusOne ? f_edgeBmin : f_edgeBmax;
	}

	Rasterizer::instance()->debugLine(v[f_edgeA], v[f_edgeB], 0xFF00);

	ret_end_p = (v[f_edgeA] + v[f_edgeB])*0.5f;
	return true;

	return false;
}

// Body

Body::Body() : debug("debug") {
	callback = 0;
	timesSplit = 0;
	breakable = true;
	breakTimeout = 1000.f;
	simulate = false;
	deleteWhenPossible = false;
	type = 0;
	damping = 0.9f;
	bcircle_cache_time = -1000.f;
	mass = 1.f;
	change_type_delay = 0.f;
	change_type = 0;
}

Body::~Body() {
	PhysicsManager::instance()->unregisterBody(this);
}

void Body::update(float frameDelta) {
	if(change_type_delay > 0.f) {
		change_type_delay -= frameDelta;
		if(change_type_delay < 0.f) {
			change_type_delay = 0.f;
			type = change_type;
		}
	}

	vec2 c = shape.getCenter();
	vec2& cameraPos = Rasterizer::instance()->getCameraPos();
	c += cameraPos;
	if(c[0] < -500.f || c[0] > SCRWIDTH+500.f || c[1] < -500.f || c[1] > SCRHEIGHT+500.f) {
		simulate = false;
		return;
	}
	simulate = true;

	breakTimeout -= frameDelta;
	if(breakTimeout < 0.f) breakTimeout = 0.f;

	if(*debug == 0) return;
	
	std::vector<vec2>& v = shape.getVertices();
	int v_n = v.size();

	for(int i=0; i<v_n; i++) {
		Rasterizer::instance()->debugPoint(v[i], 0xFF00);
	}

	std::vector<vec2> n = shape.buildNormals();
	int n_n = n.size();

	int v1=-1, v2=0;

	for(int i=0; i<n_n; i++) {
		v1++;
		v2++;
		if(v2 == n_n) v2 = 0;
		vec2 c = (v[v1] + v[v2])*0.5f;
		Rasterizer::instance()->debugLine(c, c+n[i]*20.f, 0xFFFFFF);
	}
}

float angleCenterPoint(const vec2& center, const vec2& point) {
	return atan2(point[1] - center[1], point[0] - center[0]);
}

vec2 angleSortPredicate_center;

bool angleSortPredicate(const vec2& v1, const vec2& v2) {
	if(v1 == v2) return false;
	if(angleCenterPoint(angleSortPredicate_center, v1) < angleCenterPoint(angleSortPredicate_center, v2)) return false;
	return true;
}

vec2 averageSet(const std::list<vec2>& set) {
	vec2 sum;
	for(std::list<vec2>::const_iterator v = set.begin(); v != set.end(); v++) {
		sum += *v;
	}
	sum *= 1.f/set.size();
	return sum;
}

vec2 averageSetVector(const std::vector<vec2>& set) {
	vec2 sum;
	for(std::vector<vec2>::const_iterator v = set.begin(); v != set.end(); v++) {
		sum += *v;
	}
	sum *= 1.f/set.size();
	return sum;
}

float getTriangleArea(const vec2& v1, const vec2& v2, const vec2& v3) {
	float a = (v2-v1).len();
	float b = (v3-v1).len();
	float c = (v3-v2).len();
	float s = (a+b+b)*0.5f;
	return sqrtf(s*(s-a)*(s-b)*(s-c));
}

float getSetArea(const std::list<vec2>& set) {
	std::vector<vec2> v;
	for(std::list<vec2>::const_iterator s = set.begin(); s != set.end(); s++) {
		v.push_back(*s);
	}
	int v_n = v.size();
	float area = 0.f;
	for(int i=1; i<v_n-1; i++) {
		area += getTriangleArea(v[0], v[i], v[i+1]);
	}
	return area;
}

float getSetArea(const std::vector<vec2>& set) {
	std::vector<vec2> v;
	for(std::vector<vec2>::const_iterator s = set.begin(); s != set.end(); s++) {
		v.push_back(*s);
	}
	int v_n = v.size();
	float area = 0.f;
	for(int i=1; i<v_n-1; i++) {
		area += getTriangleArea(v[0], v[i], v[i+1]);
	}
	return area;
}

Body* Body::split(const vec2& cut_a, const vec2& cut_b) {
	if(!breakable) return 0;
	if(breakTimeout != 0.f) return 0;
	breakTimeout = 1000.f;
	if(timesSplit >= 2) return 0;

	std::vector<vec2>& v = shape.getVertices();
	
	std::list<vec2> left;
	std::list<vec2> right;

	int v_n = v.size();
		
	left.push_back(cut_a);
	left.push_back(cut_b);
	
	right.push_back(cut_a);
	right.push_back(cut_b);

	for(int i=0; i<v_n; i++) {
		if(cut_b == v[i]) continue;
		if(pointLineSide(cut_a, cut_b, v[i])) {
			left.push_back(v[i]);
		} else {
			right.push_back(v[i]);
		}
	}

	if(left.size() < 3 || right.size() < 3) {	
		return 0;
	}

	angleSortPredicate_center = averageSet(left);
	left.sort(angleSortPredicate);
	angleSortPredicate_center = averageSet(right);
	right.sort(angleSortPredicate);

	float areaLeft = getSetArea(left);
	float areaRight = getSetArea(right);

	if(!(areaLeft == areaLeft)) return 0;
	if(!(areaRight == areaRight)) return 0;
	if(areaLeft < 14000.f) return 0;
	if(areaRight < 14000.f) return 0;

	Body* new1 = PhysicsManager::instance()->getFromPool();
	new1->fromConvexSet(left);
	new1->setTimesSplit(timesSplit+1);
	new1->getShape().setColor(shape.getColor());

	fromConvexSet(right);

	float area = getSetArea(shape.getVertices());
	if(area == area) {} else area = 100000.f;
	mass = area/100000.f;
	if(mass < 1.f) mass = 1.f;
	else if(mass > 3.f) mass = 3.f;
	else mass = 1.f;

	float areaNew = getSetArea(new1->getShape().getVertices());
	if(areaNew == areaNew) {} else {
		PhysicsManager::instance()->releaseToPool(new1);
		return 0;
	}
	float massNew = areaNew/100000.f;
	if(massNew < 1.f) massNew = 1.f;
	else if(massNew > 3.f) massNew = 3.f;
	else massNew = 1.f;

	new1->setMass(massNew);

	timesSplit++;
	return new1;
}

void Body::fromShape(const Shape& s) {
	shape = s;
	VerletIntegrator::instance()->initBody(this);
}

void Body::fromConvexSet(std::list<vec2>& point_set) {
	shape.fromConvexSet(point_set);
	VerletIntegrator::instance()->initBody(this);
	calculateBounds();
}

void Body::calculateBounds() {
	std::vector<vec2>& v = shape.getVertices();
	int v_n = v.size();
	float maxDist = -FLT_MAX;

	vec2 c = shape.getCenter();
	for(int i=0; i<v_n; i++) {
		float d = (v[i] - c).len2();
		if(d > maxDist) {
			maxDist = d;
			outward_vertex = i;
		}
	}

	bcircle.pos = c;
	bcircle.radius = sqrtf(maxDist);
}

BoundingCircle& Body::getBoundingCircle() {
	float time = PhysicsManager::instance()->getTime();
	if(time - bcircle_cache_time < 1.f) {
		return bcircle;
	}
	bcircle_cache_time = time;

	std::vector<vec2>& v = shape.getVertices();
	int v_n = v.size();
	if(v_n == 0) return bcircle;

	bcircle.pos = shape.getCenter();
	bcircle.radius = (v[outward_vertex] - bcircle.pos).len();
	bcircle.radius *= 1.3f;
	return bcircle;
}

// Constraint solver

ConstraintSolver* ConstraintSolver::m_instance = 0;

void ConstraintSolver::doBody(Body* body) {
	Shape& s = body->getShape();
	std::vector<vec2>& v = s.getVertices();
	std::vector<Constraint>& c = s.getConstraints();
	int c_n = c.size();

	for(int iter=0; iter<iterations; iter++)
	for(int i=0; i<c_n; i++) {
		int v1i = c[i].first.first;
		int v2i = c[i].first.second;
		float dist = c[i].second;

		vec2 v1v2 = v[v1i] - v[v2i];
		float diff = v1v2.len() - dist;
		v1v2.normalize();
		v[v1i] -= v1v2*diff*0.5f;
		v[v2i] += v1v2*diff*0.5f;
	}
}

// helper

bool rayLineSegmentIntersection(const Ray& ray, const vec2& seg_start, const vec2& seg_end, vec2& ret_intersection) {
	float rsx = ray.pos[0], rsy = ray.pos[1], rdx = ray.dir[0], rdy = ray.dir[1];
	vec2 sd = vec2(seg_end) - vec2(seg_start);

	float ssx = seg_start[0], ssy = seg_start[1], sex = sd[0], sey = sd[1];
	float r, s, d;

	if((rdy - rsy) / (rdx - rsx) == (sey - ssy) / (sex - ssx)) return false;

	d = (rdx - rsx) * (sey - ssy) - (rdy - rsy) * (sex - ssx);
	if(d > -0.001f && d < 0.001f ) return false;

    r = (((rsy - ssy) * (sex - ssx)) - (rsx - ssx) * (sey - ssy)) / d;
    s = (((rsy - ssy) * (rdx - rsx)) - (rsx - ssx) * (rdy - rsy)) / d;
     
	if(r < 0.f) return false;
	if(s < 0.f || s > 1.f) return false;

	ret_intersection = seg_start+sd*s;

	return true;
}

vec2 projectShapeOnAxis(Shape& shape, const vec2& axis) {
	std::vector<vec2>& v = shape.getVertices();
	int v_n = v.size();

	float min, max = min = axis.dot_product(v[0]);

	for(int i=1; i<v_n; i++) {
		float d = axis.dot_product(v[i]);
		if(d < min) min = d;
		else if(d > max) max = d;
	}
	return vec2(min, max);
}

float getIntervalDistance(const vec2& intervalA, const vec2& intervalB) {
	if(intervalA[0] < intervalB[0]) return intervalB[0] - intervalA[1];
	return intervalA[0] - intervalB[1];
}

bool intersectSepAxis(Shape& shapeA, Shape& shapeB, vec2& normal, float& ret_dist, bool& edgeBelongsToA, int& edge1, int& edge2, int& vert) {
	std::vector<vec2>& vA = shapeA.getVertices();
	std::vector<vec2>& vB = shapeB.getVertices();

	edge1 = -1234;
	edge2 = -1234;
	vert = -1234;

	int vAn = vA.size();
	int vBn = vB.size();

	vec2 face_v1, face_v2;
	int face_v1i, face_v2i;

	float minDist = FLT_MAX;
	ret_dist = 0.f;

	edgeBelongsToA = true;

	for(int i=0; i<vAn; i++) {
		if(i == vAn-1) {
			face_v1i = i;
			face_v2i = 0;
		} else {
			face_v1i = i;
			face_v2i = i+1;
		}
		
		face_v1 = vA[face_v1i];
		face_v2 = vA[face_v2i];

		vec2 axis(face_v1[1] - face_v2[1], face_v2[0] - face_v1[0]);
		axis.normalize();
		
		vec2 intervalA = projectShapeOnAxis(shapeA, axis);
		vec2 intervalB = projectShapeOnAxis(shapeB, axis);
		float dist = getIntervalDistance(intervalA, intervalB);

		if(dist > 0.f) {
			return false;
		}

		dist = fabs(dist);
		if(dist < minDist) {
			minDist = dist;
			normal = axis;
			edge1 = face_v1i;
			edge2 = face_v2i;
		}
	}

	for(int i=0; i<vBn; i++) {
		if(i == vBn-1) {
			face_v1i = 0;
			face_v2i = i;
		} else {
			face_v1i = i;
			face_v2i = i+1;
		}
		
		face_v1 = vB[face_v1i];
		face_v2 = vB[face_v2i];
		
		vec2 axis(face_v1[1] - face_v2[1], face_v2[0] - face_v1[0]);
		axis.normalize();
		
		vec2 intervalA = projectShapeOnAxis(shapeA, axis);
		vec2 intervalB = projectShapeOnAxis(shapeB, axis);
		float dist = getIntervalDistance(intervalA, intervalB);

		if(dist > 0.f) {
			return false;
		}

		dist = fabs(dist);
		if(dist < minDist) {
			minDist = dist;
			normal = axis;
			edge1 = face_v1i;
			edge2 = face_v2i;
			edgeBelongsToA = false;
		}
	}


	float minVertDist = FLT_MAX;

	vec2 centerA = shapeA.getCenter();
	vec2 centerB = shapeB.getCenter();

	vec2 cc = centerA - centerB;
	if(normal.dot_product(cc) < 0.f) {
		normal *= -1.f;
	}

	if(edgeBelongsToA) {
		for(int i=0; i<vBn; i++) {
			float d = -vB[i].dot_product(normal);
			if(d < minVertDist) {
				vert = i;
				minVertDist = d;
			}
		}
	} else {
		for(int i=0; i<vAn; i++) {
			float d = vA[i].dot_product(normal);
			if(d < minVertDist) {
				vert = i;
				minVertDist = d;
			}
		}
	}

	ret_dist = minDist;

	if(vert == -1234 || edge1 == -1234 || edge2 == -1234) {
	//	printf("Bug");	
		return false;
	}
	return true;
}

// Verlet integrator

VerletIntegrator* VerletIntegrator::m_instance = 0;

void VerletIntegrator::initBody(Body* body) {
	IntegratorState& state = body->getIntegratorState();
	std::vector<vec2>& v = body->getShape().getVertices();
	int v_n = v.size();

	state.v_old.clear();

	for(int i=0; i<v_n; i++) {
		state.v_old.push_back(v[i]);
	}

	state.initialized = true;
	state.sleep = false;
}

void VerletIntegrator::doBody(float deltaT, Body* body) {
	if(deltaT == 0.f) return;
	if(!body->getIntegratorState().initialized) {
		initBody(body);
	} else if(body->getIntegratorState().sleep) return;

	Shape& s = body->getShape();

	float fix_coeff = deltaT/deltaT_old;
	deltaT_old = deltaT;

	std::vector<vec2>& v = s.getVertices();
	int v_n = v.size();

    IntegratorState& state = body->getIntegratorState();

	for(int i=0; i<v_n; i++) {
		vec2 tmp = v[i];
		v[i] += (v[i] - state.v_old[i]) * body->getDamping() * fix_coeff;
		if(v[i][0] < 0.f) v[i][0] = 0.f;
		if(v[i][1] < 0.f) v[i][1] = 0.f;
		if(v[i][0] > W_WIDTH) v[i][0] = W_WIDTH;
		if(v[i][1] > W_HEIGHT) v[i][1] = W_HEIGHT;
		state.v_old[i] = tmp;
	}
}

// Physics manager

PhysicsManager* PhysicsManager::m_instance = 0;

PhysicsManager::PhysicsManager() {
	m_world_size = 0.f;
	for(int i=0; i<8192; i++) {
		bodies_pool.push_back(new Body());
	}
	time = 0.f;
	calls = 0.f;
	performanceWarning = false;
	preparation = true;

//bodies_pool = new Body[4096];
}

PhysicsManager::~PhysicsManager() {
//delete [] bodies_pool;
}

void PhysicsManager::registerBody(Body* body) {
	if(bodies.count(body) != 0) {
		LOG("Error, body already registered");
		return;
	}
	bodies.insert(body);
	VerletIntegrator::instance()->initBody(body);
//	root->addObject(body->getShape().getCenter());
}
void PhysicsManager::unregisterBody(Body* body) {
	if(bodies.count(body) == 0) {
		LOG("Error, trying to remove unregistered body");
		return;
	}
	bodies.erase(body);
}

void PhysicsManager::advanceSimulation(float deltaT) {
	time += deltaT;
	calls++;

	if(time > 5000.f && preparation) {
		preparation = false;
		if(calls < 20) performanceWarning = true;
	}

	if(performanceWarning) {
		Rasterizer::instance()->debugText(vec2(10.f, 80.f), "Performance warning! (debugger attached?)", 0xFF0000, true);
	}

	std::list<Body*> to_delete;

	for(std::set<Body*>::iterator b = bodies.begin(); b != bodies.end(); b++) {
		Body* body = (*b);
		if(body->getIntegratorState().sleep) continue;
		body->update(deltaT);
		if(!preparation && !body->doSimulate()) continue;
		VerletIntegrator::instance()->doBody(deltaT, body);
	}

	for(std::set<Body*>::iterator b = bodies.begin(); b != bodies.end(); b++) {
		Body* body = (*b);
		if(body->getIntegratorState().sleep) continue;
		if(!preparation && !body->doSimulate()) continue;
		resolveCollisions(body);
		if(body->deleteWhenPossible) {
			to_delete.push_back(body);
			continue;
		}
		ConstraintSolver::instance()->doBody(body);
	}

	for(std::list<Body*>::iterator b = to_delete.begin(); b != to_delete.end(); b++) {
		if((*b)->getBreakable())
			releaseToPool(*b);
	}
}

void PhysicsManager::resolveCollisions(Body* body) {
	std::set<Body*> bodies_cpy = bodies;
	for(std::set<Body*>::iterator b2 = bodies_cpy.begin(); b2 != bodies_cpy.end(); b2++) {
//	Body* body = (*b);
		Body* body2 = (*b2);
		if(body == body2) continue;
		if(!body2->doSimulate() && !preparation) continue;

		if(!intersectBoundingCircle(body, body2)) continue;

		vec2 nrm;
		float d;
		bool edgeIsA;
		int edge1, edge2, vert;

		bool t = intersectSepAxis(body->getShape(), body2->getShape(), nrm, d, edgeIsA, edge1, edge2, vert);
		if(!t) continue;
	
		//if(time < 5000.f) {
		//	body2->deleteWhenPossible = true;
		//	body->deleteWhenPossible = true;
		//	return;
		//}

		bool resolveA = true;
		bool resolveB = true;

		CollisionEvent cevent;

		cevent.setIntersected(body, body2, nrm, d, edgeIsA, edge1, edge2, vert);

		if(body->getCollisionCallback() != 0) {
			resolveA = body->getCollisionCallback()->receiveEvent(cevent);
			resolveA = !resolveA;
		}

		if(body2->getCollisionCallback() != 0) {
			resolveB = body2->getCollisionCallback()->receiveEvent(cevent);
			resolveB = !resolveB;
		}

		if(!resolveA || !resolveB) continue;

		vec2 displacement = nrm*d;
		
		std::vector<vec2>& pV = body->getShape().getVertices();
		std::vector<vec2>& bV = body2->getShape().getVertices();

		float inv_mass_ratio = body->getMass()/body2->getMass();
		float mass_ratio = body2->getMass()/body->getMass();

		if(edgeIsA) {
			// resolve A's edge
			if(resolveA) {
				vec2& ev1 = pV[edge1];
				vec2& ev2 = pV[edge2];

				float t;
				if(fabs(ev1[0] - ev2[0]) > fabs(ev1[1] - ev2[1])) {
					t = (bV[vert][0] - displacement[0] - ev1[0])/(ev2[0]-ev1[0]);
				} else {
					t = (bV[vert][1] - displacement[1] - ev1[1])/(ev2[1]-ev1[1]);
				}

				float lambda = 1.f / (t*t + (1.f - t)*(1.f - t));
				ev1 += displacement*(1.f-t)*lambda*mass_ratio*0.5f;
				ev2 += displacement*t*lambda*mass_ratio*0.5f;
			}
			if(resolveB) {
				// resolve B's vertex
				bV[vert] -= displacement*inv_mass_ratio*0.5f;
			}
		} else {
			if(resolveB) {
				// resolve B's edge
				vec2& ev1 = bV[edge1];
				vec2& ev2 = bV[edge2];

				float t;
				if(fabs(ev1[0] - ev2[0]) > fabs(ev1[1] - ev2[1])) {
					t = (pV[vert][0] - displacement[0] - ev1[0])/(ev2[0]-ev1[0]);
				} else {
					t = (pV[vert][1] - displacement[1] - ev1[1])/(ev2[1]-ev1[1]);
				}

				float lambda = 1.f / (t*t + (1.f - t)*(1.f - t));
				ev1 -= displacement*(1.f-t)*lambda*inv_mass_ratio*0.5f;
				ev2 -= displacement*t*lambda*inv_mass_ratio*0.5f;
			}
			if(resolveA) {
				// resolve A's vertex
				pV[vert] += displacement*mass_ratio*0.5f;
			}
		}
	}
}

Body* PhysicsManager::getFromPool() {
	Body* b = bodies_pool.front();
	bodies_pool.pop_front();
	registerBody(b);
	b->released = false;
	CInt bodies_n("bodies_n");
	*bodies_n += 1;
	return b;
}

void PhysicsManager::releaseToPool(Body* body) {
	unregisterBody(body);
	body->released = true;
	bodies_pool.push_back(body);
	CInt bodies_n("bodies_n");
	*bodies_n -= 1;
}