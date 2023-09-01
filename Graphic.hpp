#ifndef GRAPH_H
#define GRAPH_H

#include <X11/Xlib.h>


class Menu
{
	Display* _dpy;
	Window _win;
	Window _diffs[5];
	Colormap _cmap;
	XFontStruct* _fnptr;
	GC _gc;
	int _scr;
	int _depth;
	int _width;
	int _height;
	int _maxWidth;
	int _maxHeight;
	int _mode;
	void (Menu::*_show[2]) ();
public:
	Menu();
	int rekey(XEvent&);
	void create();
	void txt(Window, const char*);
	void txt(Window, int, int, const char*);
	void open();
	void show1();
	void show2();
	void start(int, int, int, int);
	~Menu();
};


#endif
