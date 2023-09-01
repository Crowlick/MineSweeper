#include <iostream>
#include <sstream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "Gameplay.hpp"
#include "Input.hpp"

Input::Input(): _width(1024), _height(480), _space(64), _lvl(0)
{
	_dpy = XOpenDisplay(NULL);                				
	_scr = DefaultScreen(_dpy);       
	_depth = DefaultDepth(_dpy, _scr);	
	int maxWidth = DisplayWidth(_dpy, _scr);
	int maxHeight = DisplayHeight(_dpy, _scr);
	XSetWindowAttributes attr;
	attr.override_redirect = False;
	attr.background_pixel = 0x689782;
	attr.event_mask = (KeyPressMask | ExposureMask);	       				                  			  				
	_win = XCreateWindow(_dpy, DefaultRootWindow(_dpy), (maxWidth - _width) / 2, (maxHeight - _height) / 2, _width, _height, 4, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	XSizeHints hint;  
	hint.flags = (PMinSize | PMaxSize | PPosition);
	hint.min_width = hint.max_width = _width;
	hint.min_height = hint.max_height = _height;
	XSetNormalHints(_dpy, _win, &hint);
	_gc = XCreateGC(_dpy, _win, 0, 0);
	_art = XCreateGC(_dpy, _win, 0, 0);
	XSetForeground(_dpy, _gc, 0x000000);
	XGCValues gval;
	unsigned long gmask = (GCLineWidth);
	gval.line_width = 10;
	XChangeGC(_dpy, _art, gmask, &gval);
	XFontStruct* fnptr;
	if((fnptr = XLoadQueryFont(_dpy, "12x24")) != NULL)
		XSetFont(_dpy, _gc, fnptr->fid);
	XFreeFont(_dpy, fnptr);
	XStoreName(_dpy, _win, "Input");
	int w = _width - 64;
	int h = _height / 4;
	_iw = w;
	_ih = h;
	attr.override_redirect = True;
	attr.background_pixel = 0x67649b;
	attr.event_mask = (ButtonPressMask | ButtonReleaseMask | ExposureMask);
	_dim[0] = XCreateWindow(_dpy, _win, (_width - w) / 2, (_height - h) / 2, w, h, 2, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	_dim[1] = XCreateWindow(_dpy, _win, (_width - w) / 2, (_height - h) / 2, w, h, 2, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	_dim[2] = XCreateWindow(_dpy, _win, (_width - w) / 2, (_height - h) / 2, w, h, 2, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	_dim[3] = XCreateWindow(_dpy, _win, (_width - w) / 2, (_height - h) / 2, w, h, 2, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	attr.event_mask |= (EnterWindowMask | LeaveWindowMask);
	_next = XCreateWindow(_dpy, _win, (_width - w) / 2, (_height - h) / 2 - 84, 128, 64, 2, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	_back = XCreateWindow(_dpy, _win, (_width - w) / 2, _height / 2 + 80, 128, 64, 2, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	attr.background_pixel = 0x62d22d;
	_start = XCreateWindow(_dpy, _win,  _width - 256, _height - 54, 226, 40, 2, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	attr.background_pixel = 0xdd2522;
	_close = XCreateWindow(_dpy, _win,  _width - 64, 10, 34, 32, 2, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	XMapWindow(_dpy, _win);
	XMapSubwindows(_dpy, _win);
	XUnmapWindow(_dpy, _start);
	_draw[0] = &Input::drawX;
	_draw[1] = &Input::drawY;
	_draw[2] = &Input::drawZ;
	_draw[3] = &Input::drawM;
	_dnum[0] = &Input::draw0;
	_dnum[1] = &Input::draw1;
	_dnum[2] = &Input::draw2;
	_dnum[3] = &Input::draw3;
	_dnum[4] = &Input::draw4;
	_dnum[5] = &Input::draw5;
	_dnum[6] = &Input::draw6;
	_dnum[7] = &Input::draw7;
	_dnum[8] = &Input::draw8;
	_dnum[9] = &Input::draw9;
	
	for (int i = 0; i < 4; i++)
		_tx[i] = 160;
	XRaiseWindow(_dpy, _dim[_lvl]);
	start();
}

void Input::drawEq()
{
	XDrawLine(_dpy, _dim[_lvl], _art, 80, _ih / 2 + 15, 140,  _ih / 2 + 15);
	XDrawLine(_dpy, _dim[_lvl], _art, 80, _ih / 2 - 15, 140,  _ih / 2 - 15);
}

void Input::drawX()
{
	XDrawLine(_dpy, _dim[0], _art, 10, 10, 60, _ih - 10);
	XDrawLine(_dpy, _dim[0], _art, 60, 10, 10, _ih - 10);
	drawEq();
}

void Input::drawY()
{
	XDrawLine(_dpy, _dim[1], _art, 10, 10, 35, _ih / 2);
	XDrawLine(_dpy, _dim[1], _art, 60, 10, 35, _ih / 2);
	XDrawLine(_dpy, _dim[1], _art, 35, _ih / 2, 35,  _ih - 10);
	drawEq();
}

void Input::drawZ()
{
	XUnmapWindow(_dpy, _start);
	XDrawLine(_dpy, _dim[2], _art, 10, 10, 64, 10);
	XDrawLine(_dpy, _dim[2], _art, 60, 10, 10, _ih - 10);
	XDrawLine(_dpy, _dim[2], _art, 7, _ih - 10, 60, _ih - 10);
	drawEq();
}

void Input::drawM()
{
	XMapWindow(_dpy, _start);
	XDrawLine(_dpy, _dim[3], _art, 10, 10, 10, _ih - 10);
	XDrawLine(_dpy, _dim[3], _art, 10, 10, 35, _ih / 2);
	XDrawLine(_dpy, _dim[3], _art, 60, 10, 35, _ih / 2);
	XDrawLine(_dpy, _dim[3], _art, 65, 10, 65, _ih - 10);
	drawEq();
}

void Input::draw1()
{
	int x = _tx[_lvl];
	int y = _ih / 2;
	XDrawLine(_dpy, _dim[_lvl], _art, x, y, x + 35, 12);
	XDrawLine(_dpy, _dim[_lvl], _art, x + 35, 10, x + 35,  _ih - 10);
	_tx[_lvl] += _space;
}

void Input::draw2()
{
	int x = _tx[_lvl] - 10;
	int y = _ih - 15;
	int ra = 28;
	int rb = 40;
	XDrawArc(_dpy, _dim[_lvl], _art, x + 33 - ra, 50 - rb, 2*ra, 2*rb, 20 * 64, 160*64);
	XDrawLine(_dpy, _dim[_lvl], _art, x + 60, 32, x + 10, y);
	XDrawLine(_dpy, _dim[_lvl], _art, x + 7, y, x + 60, y);
	_tx[_lvl] += _space;
}

void Input::draw3()
{
	int x = _tx[_lvl];
	int y = _ih / 2 - 10;
	int ra = 22;
	int rb = 22;
	int a = 90;
	XDrawArc(_dpy, _dim[_lvl], _art, x + 15 - ra, y - rb - 15, 2 * ra, 2 * rb, (0 - a) * 64, (135 + a)*64);
	XDrawArc(_dpy, _dim[_lvl], _art, x + 15 - ra, y - rb + 25, 2 * ra, 2 * rb, (0 + a)*64, -(135 + a)*64);
	_tx[_lvl] += _space;
}

void Input::draw0()
{
	int x = _tx[_lvl] + 25;
	int y = _ih / 2;
	int ra = 20;
	int rb = 40;
	XDrawArc(_dpy, _dim[_lvl], _art, x - ra, y - rb, 2 * ra, 2 * rb, 0 * 64, 360*64);
	_tx[_lvl] += _space;
}

void Input::draw4()
{
	int x = _tx[_lvl];
	int y = _ih / 2;
	XDrawLine(_dpy, _dim[_lvl], _art, x, 10, x, y);
	XDrawLine(_dpy, _dim[_lvl], _art, x-5, y, x + 25, y);
	XDrawLine(_dpy, _dim[_lvl], _art, x + 25, 10, x + 25, _ih - 10);
	_tx[_lvl] += _space;
}

void Input::draw5()
{
	int x = _tx[_lvl] + 5;
	int y = _ih / 2;
	XDrawLine(_dpy, _dim[_lvl], _art, x-5, 10, x + 30, 10);
	XDrawLine(_dpy, _dim[_lvl], _art, x, 10, x, y);
	XDrawLine(_dpy, _dim[_lvl], _art, x-5, y, x + 25, y);
	XDrawLine(_dpy, _dim[_lvl], _art, x + 25, y-5, x + 25, _ih - 10);
	XDrawLine(_dpy, _dim[_lvl], _art, x + 25, _ih - 15, x-5, _ih - 15);
	_tx[_lvl] += _space;
}

void Input::draw6()
{
	int x = _tx[_lvl] + 5;
	int y = _ih / 2;
	XDrawLine(_dpy, _dim[_lvl], _art, x-5, 10, x + 30, 10);
	XDrawLine(_dpy, _dim[_lvl], _art, x-5, y, x + 25, y);
	XDrawLine(_dpy, _dim[_lvl], _art, x + 25, y-5, x + 25, _ih - 10);
	XDrawLine(_dpy, _dim[_lvl], _art, x, _ih - 15, x, 10);
	XDrawLine(_dpy, _dim[_lvl], _art, x + 25, _ih - 15, x-5, _ih - 15);
	_tx[_lvl] += _space;
}

void Input::draw7()
{
	int x = _tx[_lvl];
	int y = _ih / 2;
	XDrawLine(_dpy, _dim[_lvl], _art, x + 10, 10, x + 54, 10);
	XDrawLine(_dpy, _dim[_lvl], _art, x + 50, 10, x+ 10, _ih - 10);
	_tx[_lvl] += _space;
}

void Input::draw8()
{
	int x = _tx[_lvl] + 5;
	int y = _ih / 2;
	XDrawLine(_dpy, _dim[_lvl], _art, x-5, 10, x + 30, 10);
	XDrawLine(_dpy, _dim[_lvl], _art, x, 10, x, _ih - 15);
	XDrawLine(_dpy, _dim[_lvl], _art, x-5, y, x + 25, y);
	XDrawLine(_dpy, _dim[_lvl], _art, x + 25, 10, x + 25, _ih - 10);
	XDrawLine(_dpy, _dim[_lvl], _art, x + 25, _ih - 15, x - 5, _ih - 15);
	_tx[_lvl] += _space;
}

void Input::draw9()
{
	int x = _tx[_lvl];
	int y = _ih / 2;
	XDrawLine(_dpy, _dim[_lvl], _art, x-5, 10, x + 30, 10);
	XDrawLine(_dpy, _dim[_lvl], _art, x, 10, x, y);
	XDrawLine(_dpy, _dim[_lvl], _art, x-5, y, x + 25, y);
	XDrawLine(_dpy, _dim[_lvl], _art, x + 25, 10, x + 25, _ih - 10);
	XDrawLine(_dpy, _dim[_lvl], _art, x + 25, _ih - 15, x + 5, _ih - 15);
	_tx[_lvl] += _space;
}

void Input::txt(Window w, const char* s)
{
	XWindowAttributes xwa;
	XGetWindowAttributes(_dpy, w, &xwa);
	std::ostringstream os;
	os << s;
	std::string conv = os.str();
	XDrawString(_dpy, w, _gc, (_iw-conv.size()*12) / 2, _height / 2 - _height / 4, conv.c_str(), conv.size());
}

void Input::drawar(Window w)
{
	XWindowAttributes xwa;
	XGetWindowAttributes(_dpy, w, &xwa);
	int wd = 64;
	int x = (xwa.width - wd) / 2;
	XDrawLine(_dpy, w, _art, x-3, xwa.height / 2, x + wd + 3, xwa.height / 2);
	if (w == _next)
	{
		XDrawLine(_dpy, w, _art, x + wd, xwa.height / 2, x + wd / 2, xwa.height / 2 + 20);
		XDrawLine(_dpy, w, _art, x + wd, xwa.height / 2, x + wd / 2, xwa.height / 2 - 20);
	}
	else
	{
		XDrawLine(_dpy, w, _art, x, xwa.height / 2, x + wd / 2, xwa.height / 2 + 20);
		XDrawLine(_dpy, w, _art, x, xwa.height / 2, x + wd / 2, xwa.height / 2 - 20);
	}
}

void Input::expo()
{
	txt(_win, "START TYPING!!!");
	XWindowAttributes xwa;
	XGetWindowAttributes(_dpy, _start, &xwa);
	XDrawString(_dpy, _start, _gc, (xwa.width - 5*12) / 2, (xwa.height + 24) / 2, "START", 5);
	drawX();
	XDrawLine(_dpy, _close, _art, 0, 0, 33, 32);
	XDrawLine(_dpy, _close, _art, 0, 32, 34, 0);
	drawar(_next);
	drawar(_back);
}

int Input::rekey(XEvent& ev)
{
	KeySym ks = XLookupKeysym(&(ev.xkey), 0);
	if (ks == XK_Escape) 	
		return 'E';
	if (ks == XK_Return)
	{	
		if (_lvl == 3)
			return go();
		return slide(_lvl + 1);
	}
	if (ks == XK_BackSpace)
	{
		if (_tx[_lvl] <= 160)
			return 0;
		XSetForeground(_dpy, _art, 0x67649b);
		_tx[_lvl] -= _space;
		(this->*_dnum[_scrn[_lvl][_scrn[_lvl].size() - 1]])();
		_tx[_lvl] -= _space;
		_scrn[_lvl].pop_back();
		XSetForeground(_dpy, _art, 0x000000);
	}
	if (_scrn[_lvl].size() > 1)
		return 0;
	if (ks == XK_1)
	{
		(this->*_dnum[1])();
		_scrn[_lvl].push_back(1);
	}
	else if (ks == XK_2)
	{
		(this->*_dnum[2])();
		_scrn[_lvl].push_back(2);
	}
	else if (ks == XK_3)
	{
		(this->*_dnum[3])();
		_scrn[_lvl].push_back(3);
	}
	else if (ks == XK_4)
	{
		(this->*_dnum[4])();
		_scrn[_lvl].push_back(4);
	}
	else if (ks == XK_5)
	{
		(this->*_dnum[5])();
		_scrn[_lvl].push_back(5);
	}
	else if (ks == XK_6)
	{
		(this->*_dnum[6])();
		_scrn[_lvl].push_back(6);
	}
	else if (ks == XK_7)
	{
		(this->*_dnum[7])();
		_scrn[_lvl].push_back(7);
	}
	else if (ks == XK_8)
	{
		(this->*_dnum[8])();
		_scrn[_lvl].push_back(8);
	}
	else if (ks == XK_9)
	{
		(this->*_dnum[9])();
		_scrn[_lvl].push_back(9);
	}
	else if (ks == XK_0)
	{
		(this->*_dnum[0])();
		_scrn[_lvl].push_back(0);
	}
	return(0);        							
}

int Input::slide(int l)
{
	if (l < 0 || l >= 4)
		return 0;
	_lvl = l;
	XRaiseWindow(_dpy, _dim[_lvl]);
	int x = _tx[_lvl];
	_tx[_lvl] = 160;
	for (auto i : _scrn[_lvl])
		(this->*_dnum[i])();
	_tx[_lvl] = x;
	(this->*_draw[_lvl])();
	return 0;
}

int Input::parse(std::vector<int>& a)
{
	int m = 1;
	int sum = 0;
	for (int i = a.size() - 1; i >= 0; i--, m*=10)
		sum += a[i] * m;
	return sum;
}

int Input::go()
{
	int x = parse(_scrn[0]);
	int y = parse(_scrn[1]);
	int z = parse(_scrn[2]);
	int m = parse(_scrn[3]);
	if (x <= 3 || y <= 0 || z <= 0 || m >= (x * y * z - 27) || m < 0)
	{
		XDrawString(_dpy, _win, _gc, (_iw - 21*12)/2, _height / 2 + 100, "Wrong size! (X >= 4!)", 21);
		return 0;
	}
	XUnmapWindow(_dpy, _win);
	XFlush(_dpy);
	Game g(x, y, z, m);
	g.start();
	return 1;
}

void Input::start()
{
	int done = 0;               
	XEvent event;      
	while(done == 0) 
	{                
		XNextEvent(_dpy, &event);      
		switch(event.type) 
		{          
			case Expose:
				expo();
				break;
			case ButtonRelease:
				if (event.xbutton.button == 1)
				{
					if (event.xbutton.window == _next)
						slide(_lvl + 1);
					else if (event.xbutton.window == _back)
						slide(_lvl - 1);
					else if (event.xbutton.window == _close)
						done = 1;
					else if (event.xbutton.window == _start)
						done = go();
				}
				break;
			case KeyPress:
				done = rekey(event);
				break;
			default: break;
		}
	}   
	return;
}

Input::~Input()
{
	XFreeGC(_dpy, _gc);
	XFreeGC(_dpy, _art);
	XDestroyWindow(_dpy, _win);
	XCloseDisplay(_dpy);
}
