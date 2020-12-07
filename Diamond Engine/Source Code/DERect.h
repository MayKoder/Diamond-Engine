#pragma once

struct DERect
{
	int x, y, w, h;
	DERect(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}
	DERect() : x(0), y(0), w(0), h(0) {}
	bool Contains(DERect b);
};