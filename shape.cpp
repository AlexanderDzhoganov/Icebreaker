#include "common.h"

using namespace Tmpl8;

// Helpers

float pointLineTest(const vec2& l_start, const vec2& l_end, const vec2& p) {
	return ((l_start[0]-l_end[0])*(p[1]-l_end[1])-(p[0]-l_end[0])*(l_start[1]-l_end[1]));
};

// Shape

void Shape::fromConvexSet(std::list<vec2>& point_set) {
	int size = point_set.size();
	vertices.clear();
	edges.clear();
	constraints.clear();

	angleSortPredicate_center = averageSet(point_set);
	point_set.sort(angleSortPredicate);

	int i = 0;
	for(std::list<vec2>::const_iterator p = point_set.begin(); p != point_set.end(); p++) {
		int edgeA = i;
		int edgeB = i+1;

		if(i==size-1) {
			edgeA = 0;
			edgeB = i;
		}
		
		vertices.push_back(*p);
		edges.push_back(std::pair<int, int>(edgeA, edgeB));
		i++;
	}
	for(int x=0; x<size; x++) for(int y=x; y<size; y++) {
		if(x == y) continue;
		float len = (vertices[x] - vertices[y]).len();
		constraints.push_back(std::pair<std::pair<int, int>, float>(std::pair<int, int>(x, y), len));
	}
}

std::vector<vec2>& Shape::getVertices() {
	return vertices;
}

std::vector<vec2> Shape::buildNormals() { 
	int v_n = vertices.size();
	std::vector<vec2> normals;
	vec2 c = getCenter();
	c.normalize();

	for(int i=0; i<v_n; i++) {
		int edgeA = i;
		int edgeB = i+1;

		if(i==v_n-1) {
			edgeA = i;
			edgeB = 0;
		}

		vec2 edge = vertices[edgeB] - vertices[edgeA];
	
		normals.push_back(vec2(-edge[1], edge[0]));
		
		normals.back().normalize();
	}
	return normals; 
}

std::vector<Constraint>& Shape::getConstraints() {
	return constraints;
}

vec2 Shape::getCenter() {
	vec2 center;
	int v_n = vertices.size();

	for(int i=0; i<v_n; i++) {
		center += vertices[i];
	}

	center *= 1.f/float(v_n);
	return center;
}