#include <iostream>
#include <algorithm>
#include <X11/Xutil.h>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include "Cell.hpp"

Game::Game(int x, int y, int z, int ms): _x(x), _y(y), _z(z), _m(ms), _lvl(0), _gap(80), _size(64), _started(0), _gmvr(0)
{
	m = _m;
	if (x <= 3 || y <= 0 || z <= 0 || m >= (x * y * z) || m < 0)
	{
		std::cout << "Wrong size\n";
		exit(1);
	}
	_dpy = XOpenDisplay(NULL);
	int cs = Cell::gs();
	int cb = Cell::gb();
	_cor = Cell::gs() + 2 * Cell::gb();                    				
	_scr = DefaultScreen(_dpy);       
	_depth = DefaultDepth(_dpy, _scr);	
	_cmap = DefaultColormap(_dpy, _scr);
	_gc = XDefaultGC(_dpy, _scr);
	XSetForeground(_dpy, _gc, 0xFF0000);
	int maxWidth = DisplayWidth(_dpy, _scr);
	int maxHeight = DisplayHeight(_dpy, _scr);
	XSetWindowAttributes attr;      
	attr.override_redirect = False;
	attr.background_pixel = 0x689782;
	attr.event_mask = (KeyPressMask | ExposureMask);	       				                  	
	width = x * cs +  2*x * cb;
	int height = y * cs + 2*y * cb + _gap + 32;			   				
	_win = XCreateWindow(_dpy, DefaultRootWindow(_dpy), (maxWidth - width) / 2, (maxHeight - height) / 2, width, height, 4, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	_art = XCreateGC(_dpy, _win, 0, 0);
	XGCValues gval;
	unsigned long gmask = (GCLineWidth);
	gval.line_width = 3;
	XChangeGC(_dpy, _art, gmask, &gval);
	attr.background_pixel = 0x969069;
	attr.override_redirect = True;
	attr.event_mask = (ExposureMask);
	_mines = XCreateWindow(_dpy, _win, width - _size - 6, 0, _size, _size, 3, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	_lvls = XCreateWindow(_dpy, _win, 0, 0, _size, _size, 3, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	attr.event_mask |= (EnterWindowMask | LeaveWindowMask | ButtonPressMask | ButtonReleaseMask);
	_reset = XCreateWindow(_dpy, _win, (width - 38)/2, 0, 32, 32, 3, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	_exit = XCreateWindow(_dpy, _win, 0, height-32, width-2, 30, 1, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	XFontStruct* fnptr;
	if((fnptr = XLoadQueryFont(_dpy, "12x24")) != NULL)
		XSetFont(_dpy, _gc, fnptr->fid);
	XSizeHints hint;  
	hint.flags = (PMinSize | PMaxSize | PPosition);
	hint.min_width = hint.max_width = width;
	hint.min_height = hint.max_height = height;
	XSetNormalHints(_dpy, _win, &hint);
	XStoreName(_dpy, _win, "MineSweeper");
	XMapWindow(_dpy, _win);
	XFreeFont(_dpy, fnptr);
	create();
}


void Game::addmines(int sx, int sy)
{
	std::vector<int> arr(_x * _y * _z);
	for (int i = 0; i < arr.size(); i++)
		arr[i] = i;
	for (int j = -1; j < 2; j++)
		for (int k = -1; k < 2; k++)
			for (int n = -1; n < 2; n++)
			{
				int nx = sx + j;
				int ny = sy + k;
				int nz = _lvl + n;
				if (nx >= 0 && ny >= 0 && nz >= 0 && nz < _z && nx < _x && ny < _y)
				{
					int number = (_x * _y * nz) + _x * ny + nx;
					int i = 0;
					for (i = 0; arr[i] < number; i++);
						arr.erase(arr.begin() + i);
				}
			}
	std::random_shuffle(arr.begin(), arr.end());
	int cs = Cell::gs();
	int cb = Cell::gb();
	for (int i = 0; i < m; i++)
	{
		int x = arr[i] % _x;
		int y = (arr[i] % (_x * _y)) / _x;
		int z = arr[i] / (_x * _y);
		for (int j = -1; j < 2; j++)
			for (int k = -1; k < 2; k++)
				for (int n = -1; n < 2; n++)
				{
					int nx = x + j;
					int ny = y + k;
					int nz = z + n;
					if (nx >= 0 && ny >= 0 && nz >= 0 && nz < _z && nx < _x && ny < _y)
						_field[nx][ny][nz]->modify();
				}
		delete _field[x][y][z];
		_field[x][y][z] = new Mine(*this, x * (cs + 2 * cb), y * (cs + 2 * cb)+ _gap, z);
	}
}

void Game::create()
{
	_field = new Cell***[_x];
	for (int i = 0; i < _x; i++)
		_field[i] = new Cell**[_y];
	for (int i = 0; i < _x; i++)
		for (int j = 0; j < _y; j++)
			_field[i][j] = new Cell*[_z];
	int cs = Cell::gs();
	int cb = Cell::gb();   
	for (int i = 0; i < _x*_y*_z; i++)
	{
		int x = i % _x;
		int y = (i % (_x * _y)) / _x;
		int z = i / (_x * _y);
		_field[x][y][z] = new Number(*this, x * (cs + 2 * cb), y * (cs + 2 * cb) + _gap, z);
	}
	XMapWindow(_dpy, _lvls);
	XMapWindow(_dpy, _mines);
	XMapWindow(_dpy, _reset);
	XMapWindow(_dpy, _exit);
	for (int i = 0; i < _x; i++)
		for (int j = 0; j < _y; j++)
			XMapWindow(_dpy, _field[i][j][_lvl]->gw());
}

void Game::txt(Window w, int l)
{
	XWindowAttributes xwa;
	XGetWindowAttributes(_dpy, w, &xwa);
	std::ostringstream os;
	os << l;
	std::string conv = os.str();
	XDrawString(_dpy, w, _gc, (xwa.width - conv.size() * 12 +4)/2, (xwa.height + 24) / 2, conv.c_str(), conv.size());
}

void Game::draw()
{
	int r = 12;
	int a = 15;
	XDrawArc(_dpy, _reset, _art, 16 - r,16 - r, r * 2, r * 2, (90+a)*64, (180-2*a)*64);
	XDrawArc(_dpy, _reset, _art, 16 - r,16 - r, r * 2, r * 2, (90-a)*64, - (180-2*a)*64);
	XDrawLine(_dpy, _reset, _art, 16 + 4, 16+11, 16 + 4 + 3, 16+11 - 7);
	XDrawLine(_dpy, _reset, _art, 16 + 4, 16+11, 16 + 4 + 9, 16+11-1);
	XDrawLine(_dpy, _reset, _art, 16 - 4, 16-11, 16 - 4 - 9, 16-11+1);
	XDrawLine(_dpy, _reset, _art, 16 - 4, 16-11, 16 - 4 - 3, 16-11+7);
}

void Game::draw2()
{
	XDrawLine(_dpy, _exit, _art, width / 2-26, 16, width / 2 + 32, 16);
	XDrawLine(_dpy, _exit, _art, width / 2-26, 16, width / 2-5, 16 + 5);
	XDrawLine(_dpy, _exit, _art, width / 2-26, 16, width / 2-5, 16 - 5);
}

void Game::clear()
{
	m = _m;
	_lvl = 0;
	XClearWindow(_dpy, _lvls);
	XClearWindow(_dpy, _mines);
	for (int i = 0; i < _x; i++)
		for (int j = 0; j < _y; j++)
			for (int k = 0; k < _z; k++) 
				delete _field[i][j][k];
	for (int i = 0; i < _x; i++)
		for (int j = 0; j < _y; j++)
			delete[] _field[i][j];
	for (int i = 0; i < _x; i++)
		delete[] _field[i];
	delete[] _field;
}

void Game::gameover()
{
	int check = 0;
	for (int i = 0; i < _x; i++)
		for (int j = 0; j < _y; j++)
			for (int k = 0; k < _z; k++)
				if (_field[i][j][k]->gn() == -1)
					_field[i][j][k]->open();
}

Game::~Game()
{
	clear();
	XFreeGC(_dpy, _art);
	XDestroyWindow(_dpy, _mines);
	XDestroyWindow(_dpy, _lvls);
	XDestroyWindow(_dpy, _reset);
	XDestroyWindow(_dpy, _exit);
	XDestroyWindow(_dpy, _win);
	XCloseDisplay(_dpy);
}
