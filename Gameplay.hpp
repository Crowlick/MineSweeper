#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "Graphic.hpp"
#include "Cell.hpp"

class Cell;

class Game
{
	Display* _dpy;
	Window _win;
	Window _mines;
	Window _reset;
	Window _lvls;
	Window _exit;
	Colormap _cmap;
	GC _gc;
	GC _art;
	Cell**** _field;
	std::pair<int, int> _curcor;
	int _scr;       
	int _depth;	
	int _gap;
	int width;
	int _started;
	int _x;
	int _y;
	int _z;
	int _lvl;
	int m;
	int _m;
	int _cor;
	int _size;
	int _gmvr;
public:
	Game(int, int, int, int);
	int gx() const {return _x;}
	int gy() const {return _y;}
	int gz() const {return _z;}
	int gp() const {return _gap;}
	Window gw() const {return _win;}
	Display* gd() const {return _dpy;}
	Colormap gcm() const {return _cmap;}
	int gde() const {return _depth;}
	Cell**** f() const {return _field;}
	void addmines(int, int);
	void pulse(Window, void (Game::*) ());
	void pulse(Window, void (Game::*) (), unsigned long);
	void txt(Window, int);
	void clear();
	void draw2();
	void draw();
	void create();
	void start();
	void gameover();
	void expo();
	void getcor(XEvent&);
	int input(XEvent&);
	int left(XEvent&);
	int right(XEvent&);
	int mouse(XEvent&);
	void light(XEvent&);
	void rlight(XEvent&);
	int up();
	int rekey(XEvent&);
	int down();
	~Game();
};

#endif
