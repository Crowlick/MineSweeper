#include <iostream>
#include <sstream>
#include "Cell.hpp"

int Cell::_size = 32;
int Cell::_bold = 2;

unsigned long Cell::grad(double per)
{
	XColor rgb;
	if (per <= 0.25)
	{
		rgb.red = 0;
		rgb.green = 0xFFFF * per / 0.25;
		rgb.blue = 0xFFFF;
	}
	else if (per <= 0.5)
	{
		rgb.red = 0;
		rgb.green = 0xFFFF;
		rgb.blue = 0xFFFF - 0xFFFF * per / 0.5;
	}
	else if (per <= 0.75)
	{
		rgb.red = 0xFFFF * per / 0.75;
		rgb.green = 0xFFFF;
		rgb.blue = 0;
	}
	else if (per <= 1)
	{
		rgb.red = 0xFFFF;
		rgb.green = 0xFFFF - 0xFFFF * per;
		rgb.blue = 0;
	}
	else
	{
		rgb.red = 0xFFFF;
		rgb.green = 0;
		rgb.blue = 0;
	}
	XAllocColor(_loc.gd(), _loc.gcm(), &rgb);
	return rgb.pixel;
}

Cell::Cell(Game& env, int x, int y, int z): _z(z), _dpy(env.gd()), _loc(env), _op(0), _f(0)
{
	int cor = Cell::gs() + 2 * Cell::gb();
	_x = x / cor;
	_y = (y - _loc.gp()) / cor;	
	XSetWindowAttributes attr; 
	XGCValues gval; 
	attr.override_redirect = True;
	attr.background_pixel = 0x7C8281;
	attr.event_mask = (ButtonPressMask | ButtonReleaseMask | ExposureMask);
	_cell = XCreateWindow(env.gd(), env.gw(), x, y, _size, _size, _bold, env.gde(), InputOutput, CopyFromParent, (CWOverrideRedirect | CWBackPixel | CWEventMask),  &attr);
	_gc = XCreateGC(_dpy, _cell, 0, 0);
	_gcf = XCreateGC(_dpy, _cell, 0, 0);
	_gcb = XCreateGC(_dpy, _cell, 0, 0);
	XSetForeground(_dpy, _gcf, 0x7C8281);
	XSetForeground(_dpy, _gc, 0x000000);
	XSetForeground(_dpy, _gcb, 0x7C8281);
	XFontStruct* fnptr;
	if((fnptr = XLoadQueryFont(_dpy, "12x24")) != NULL)
		XSetFont(_dpy, _gc, fnptr->fid);
	XFreeFont(_dpy, fnptr);
}

void Cell::drawf()
{
	XFillArc(_loc.gd(), _cell, _gc, _size / 2- 5, _size / 2- 5, 10, 10, 0, 360*64);
}

int Cell::setf()
{
	_f = (_f + 1) % 2;
	if (_f)
	{
		drawf();
		return 1;
	}
	XFillArc(_loc.gd(), _cell, _gcf, _size / 2 - 5, _size / 2- 5, 10, 10, 0, 360*64);
	return -1;
}

void Cell::txt(int l) 
{
	XWindowAttributes xwa;
	XGetWindowAttributes(_dpy, _cell, &xwa);
	std::ostringstream os;
	os << l;
	std::string conv = os.str();
	XDrawString(_dpy, _cell, _gc, (xwa.width - conv.size() * 12)/2, (xwa.height + 24) / 2, conv.c_str(), conv.size());
}

int Number::open()
{
	_op = 1;
	if (_num)
	{
		XSetWindowBackground(_dpy, _cell, 0xbabdbc);
		XSetForeground(_dpy, _gcb,  0xbabdbc);
		XClearWindow(_dpy, _cell);
		XSetForeground(_dpy, _gc, grad(_num / 12.0));
		txt(_num);
		XFlush(_dpy);
		return 0;
	}
	XSetWindowBackground(_dpy, _cell, 0xE0E6E4);
	XSetForeground(_dpy, _gcb, 0xE0E6E4);
	XClearWindow(_dpy, _cell);
	ropen();
	return 0;
}

void Number::ropen()
{
	for (int j = -1; j < 2; j++)
		for (int k = -1; k < 2; k++)
			for (int n = -1; n < 2; n++)
			{
				int nx = _x + j;
				int ny = _y + k;
				int nz = _z + n;
				if (nx >= 0 && ny >= 0 && nz >= 0 && nz < _loc.gz() && nx < _loc.gx() && ny < _loc.gy())
				{
					if (!_loc.f()[nx][ny][nz]->isopen() && !_loc.f()[nx][ny][nz]->gf())
						_loc.f()[nx][ny][nz]->open();
				}
			}
	return;
}

int Mine::open()
{
	_op = 1;
	XSetWindowBackground(_dpy, _cell, 0xFF0000);
	XSetForeground(_dpy, _gcb, 0xFF0000);
	XClearWindow(_dpy, _cell);
	return 1;
}

