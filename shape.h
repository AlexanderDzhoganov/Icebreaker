#ifndef __SHAPE_H
#define __SHAPE_H

float pointLineTest(const vec2& l_start, const vec2& l_end, const vec2& p);

void convexHullFromPointSet(std::vector<vec2>& set, std::vector<vec2>& r_vert, std::vector<std::pair<int, int> >& r_edges);

typedef std::pair<std::pair<int, int>, float> Constraint;

class Shape {
	std::vector<vec2> vertices;
	std::vector<std::pair<int, int> > edges;
	std::vector<Constraint> constraints;

	unsigned long color;

	public:
	Shape() {}
	~Shape() {}
	void fromConvexSet(std::list<vec2>& point_set);
	std::vector<vec2>& getVertices();
	std::vector<vec2> getTransformedVertices(const vec2& transl) {
		std::vector<vec2> t_vertices;
		int v_n = vertices.size();
		for(int i=0; i<v_n; i++) {
			t_vertices.push_back(vertices[i]+transl);
		}
		return t_vertices;
	}
	std::vector<vec2> buildNormals();
	std::vector<Constraint>& getConstraints();
	vec2 getCenter();
	unsigned long getColor() { return color; }
	void setColor(unsigned long c) { color = c; }
};

#endif