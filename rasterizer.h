#ifndef __RASTERIZER_H
#define __RASTERIZER_H

typedef unsigned long Pixel;

struct Edge {
	int X1, Y1, X2, Y2;

	Edge(int x1, int y1, int x2, int y2) {
		if(y1 < y2) {
			X1 = x1;
			X2 = x2;
			Y1 = y1;
			Y2 = y2;
		} else {
			X1 = x2;
			X2 = x1;
			Y1 = y2;
			Y2 = y1;
		}
	}
};

struct Span {
	int min, max;
	Span(float x1, float x2) {
		if(x1 < x2) {
			min = x1; max = x2;
		} else {
			min = x2; max = x1;
		}
	};
};

class Rasterizer {
	static Rasterizer* m_instance;

	Tmpl8::Surface* scr;

	CInt debug;

	vec2 cameraPos;

	public:
	Rasterizer() : debug("debug") {
		
	}
	~Rasterizer() {}
	void setScreen(Tmpl8::Surface* screen) {
		scr = screen;
	}
	void drawSpan(const Span& span, int y, Pixel color);
	void drawSpans(const Edge& e1, const Edge& e2, Pixel color);
	void doTriangle(const vec2& v1, const vec2& v2, const vec2& v3, Pixel color);
	void doShape(Shape& s, Pixel color);
	void debugPoint(const vec2& p, Pixel color);
	void debugLine(const vec2& s, const vec2& e, Pixel color);
	void debugCircle(const vec2& p, float r, Pixel folor);
	void debugWireframe(Shape& s, Pixel color);
	void debugText(const vec2& p, char* str, Pixel color, bool override_debug=false);
	void setCameraPos(const vec2& campos) { cameraPos = campos; }
	vec2& getCameraPos() { return cameraPos; }
	static Rasterizer* instance() {
		if(m_instance == 0)
			m_instance = new Rasterizer();
		return m_instance;
	}
};


#endif