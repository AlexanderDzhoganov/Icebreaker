#include "common.h"

// Rasterizer

Rasterizer* Rasterizer::m_instance = 0;

void Rasterizer::drawSpan(const Span& span, int y, Pixel color) {
	if(y < 0 || y >= SCRHEIGHT) return;

	int xdiff = span.max - span.min;
	int min = span.min;
	int max = span.max;
	if(min == max) return;

	if(min < 0) min = 0;
	if(max > SCRWIDTH) max = SCRWIDTH-1;
	for(int x = min; x < max; x++) {
		scr->GetBuffer()[x+y*SCRWIDTH] = color;
	}
}

void Rasterizer::drawSpans(const Edge& e1, const Edge& e2, Pixel color) {
	float e1ydiff = e1.Y2 - e1.Y1;

	if(e1ydiff == 0.f) return;

	float e2ydiff = e2.Y2 - e2.Y1;

	if(e2ydiff == 0.f) return;

	float e1xdiff = e1.X2 - e1.X1;
	float e2xdiff = e2.X2 - e2.X1;

	float factor1 = (e2.Y1 - e1.Y1) / e1ydiff;
	float factorStep1 = 1.f / e1ydiff;
	float factor2 = 0.f;
	float factorStep2 = 1.f / e2ydiff;

	for(int y = e2.Y1; y < e2.Y2; y++) {
		Span span(e1.X1+int(e1xdiff*factor1), e2.X1+int(e2xdiff*factor2));
		drawSpan(span, y, color);
		factor1 += factorStep1;
		factor2 += factorStep2;
	}	
}

void Rasterizer::doTriangle(const vec2& v1, const vec2& v2, const vec2& v3, Pixel color) {
	Edge edges[3] = { Edge(v1[0], v1[1], v2[0], v2[1]), Edge(v2[0], v2[1], v3[0], v3[1]), Edge(v3[0], v3[1], v1[0], v1[1]) };
	
	int maxlen = 0.f;
	int longEdge = 0;

	for(int i=0; i<3; i++) {
		int len = edges[i].Y2 - edges[i].Y1;
		if(len > maxlen) {
			maxlen = len;
			longEdge = i;
		}
	}

	int shortEdge1 = (longEdge + 1) % 3;
	int shortEdge2 = (longEdge + 2) % 3;

	drawSpans(edges[longEdge], edges[shortEdge1], color);
	drawSpans(edges[longEdge], edges[shortEdge2], color);
}

void Rasterizer::doShape(Shape& s, Pixel color) {
	std::vector<vec2>& vertices = s.getTransformedVertices(cameraPos);
	int v_n = vertices.size();
	if(v_n == 0) return;

	vec2 v = vertices[0];
	

	for(int i=1; i<v_n-1; i++) {
		Rasterizer::instance()->doTriangle(v, vertices[i], vertices[i+1], color);
	}
}

void Rasterizer::debugPoint(const vec2& p, Pixel color) {
	if(!*debug) return;
	int x = p[0], y = p[1];
//	if(x<1 || y<1 || x > SCRWIDTH-2 || y > SCRHEIGHT-2) return;
	scr->Plot(x, y, color);
	scr->Plot(x-1, y, color);
	scr->Plot(x, y-1, color);
	scr->Plot(x+1, y, color);
	scr->Plot(x, y+1, color);
	scr->Plot(x-1, y-1, color);
	scr->Plot(x+1, y+1, color);
}

void Rasterizer::debugLine(const vec2& s, const vec2& e, Pixel color) {
	if(!*debug) return;
	int s_x = s[0], s_y = s[1];
	int e_x = e[0], e_y = e[1];
	scr->Line(s_x, s_y, e_x, e_y, color);
}

void Rasterizer::debugCircle(const vec2& p, float r, Pixel folor) {

}

void Rasterizer::debugWireframe(Shape& s, Pixel color) {

}

void Rasterizer::debugText(const vec2& p, char* str, Pixel color, bool override_debug) {
	if(!*debug && !override_debug) return;
	scr->Print(str, p[0], p[1], color);
}