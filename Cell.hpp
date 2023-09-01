#ifndef CELL_H
#define CELL_H


#include <X11/Xlib.h>

#include "Gameplay.hpp"
#include "Graphic.hpp"


class Game;

class Cell
{
protected:
	int _x;
	int _y;
	int _z;
	int _f;
	static int _size;
	static int _bold;
	int _op;
	Window _cell;
	Display* _dpy;
	Game& _loc;
	GC _gc;
	GC _gcf;
	GC _gcb;
public:
	Cell(Game&, int, int, int);
	static int gs() {return _size;}
	static int gb() {return _bold;}
	Window gw() {return _cell;}
	int setf();
	void drawf();
	int gf() {return _f;}
	virtual void modify() = 0;
	virtual int open() = 0;
	virtual int gn() = 0;
	virtual void ropen() = 0;
	int isopen() {return _op;}
	unsigned long grad(double per);
	GC gg() {return _gc;}
	GC gcb() {return _gcb;}
	void txt(int);
	virtual ~Cell() {XDestroyWindow(_dpy, _cell); XFreeGC(_dpy, _gc); XFreeGC(_dpy, _gcf); XFreeGC(_dpy, _gcb);} 
};

class Number: public Cell
{
	int _num;
public:
	Number(Game& env, int x, int y, int z): Cell(env, x, y, z), _num(0) {}
	int gn() {return _num;}
	void mknum();
	void modify() {_num++;}
	int open();
	void ropen();
};

class Mine: public Cell
{
public:
	Mine(Game& env, int x, int y, int z): Cell(env, x, y, z) {}
	void modify() {}
	int gn() {return -1;}
	int open();
	void ropen() {}
};

#endif
