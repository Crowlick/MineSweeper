#include <iostream>
#include <X11/keysym.h>
#include "Cell.hpp"


void Game::expo()
{
	txt(_mines, m);
	txt(_lvls, _lvl);
	draw();
	draw2();
	for (int i = 0; i < _x; i++)
		for (int j = 0; j < _y; j++)
		{
			if (_field[i][j][_lvl]->gn() > 0 && _field[i][j][_lvl]->isopen())
				_field[i][j][_lvl]->txt(_field[i][j][_lvl]->gn());
			if (_field[i][j][_lvl]->gf())
				_field[i][j][_lvl]->drawf();
		}
}

int Game::rekey(XEvent& ev)
{
	KeySym ks = XLookupKeysym(&(ev.xkey), 0);
	if (ks == XK_Escape)     						
		return 'E';
	if (ks == XK_w)
		up();
	if (ks == XK_s)
		down();
	return(0);        							
}

void Game::getcor(XEvent& event)
{
	XWindowAttributes xwa;
	XGetWindowAttributes(_dpy, event.xbutton.window, &xwa); 
	_curcor.first = xwa.x / _cor;
	_curcor.second = (xwa.y - _gap) / _cor;
}

int Game::left(XEvent& event)
{
	if (event.xbutton.window == _reset)
	{
		clear();
		create();
		_started = 0;
		_gmvr = 0;
		return 0;
	}
	if (event.xbutton.window == _exit)
		return 1;
	if (_gmvr)
		return 0;
	getcor(event);
	int x = _curcor.first;
	int y = _curcor.second;
	if (!_started)
	{
		addmines(x, y);
		XMapSubwindows(_dpy, _win);
		for (int i = 0; i < _x; i++)
			for (int j = 0; j < _y; j++)
				XRaiseWindow(_dpy, _field[i][j][_lvl]->gw());
		_started = 1;
	}
	if (!_field[x][y][_lvl]->gf() && !_field[x][y][_lvl]->isopen())
		_gmvr = _field[x][y][_lvl]->open();
	if (_gmvr)
		gameover();
	mouse(event);
	return 0;
}

int Game::right(XEvent& event)
{
	if (event.xbutton.window == _reset || event.xbutton.window == _exit)
		return 0;
	if (_gmvr)
		return 0;
	getcor(event);
	int x = _curcor.first;
	int y = _curcor.second;
	if (!_field[x][y][_lvl]->isopen())
		m -= _field[x][y][_lvl]->setf();
	XClearWindow(_dpy, _mines);
	txt(_mines, m);
	return 0;
}

int Game::mouse(XEvent& event)
{
	if (event.xbutton.window == _reset || event.xbutton.window == _exit)
		return 0;
	if (_gmvr)
		return 0;
	getcor(event);
	int x = _curcor.first;
	int y = _curcor.second;
	int fx = 0;
	int flags = 0;
	XSetForeground(_dpy, _gc, 0xbabdbc);
	for (int j = -1; j < 2; j++)
	{
		for (int k = -1; k < 2; k++)
			for (int n = -1; n < 2; n++)
			{
				int nx = x + k;
				int ny = y + n;
				int nz = _lvl + j;
				if (nx >= 0 && ny >= 0 && nz >= 0 && nz < _z && nx < _x && ny < _y)
				{
					if (_field[nx][ny][nz]->gf())
						flags++;
					XFillArc(_dpy, _field[nx][ny][_lvl]->gw(), _field[nx][ny][_lvl]->gcb(), fx, 0, 5, 5, 0, 64*360);
				}
			}
			fx += 10;
	}
	if (_field[x][y][_lvl]->isopen() && (flags == _field[x][y][_lvl]->gn()))
		_field[x][y][_lvl]->ropen();
	XSetForeground(_dpy, _gc, 0xFF0000);
	return 0;
}

int Game::up()
{
	if (_lvl >= _z - 1)
		return 0;
	for (int i = 0; i < _x; i++)
		for (int j = 0; j < _y; j++)
			XUnmapWindow(_dpy, _field[i][j][_lvl]->gw());
	_lvl++;
	XClearWindow(_dpy, _lvls);
	txt(_lvls, _lvl);
	for (int i = 0; i < _x; i++)
		for (int j = 0; j < _y; j++)
		{
			XMapWindow(_dpy, _field[i][j][_lvl]->gw());
			XRaiseWindow(_dpy, _field[i][j][_lvl]->gw());
		}
	return 0;
}

int Game::down()
{
	if (_lvl <= 0)
		return 0;
	for (int i = 0; i < _x; i++)
		for (int j = 0; j < _y; j++)
			XUnmapWindow(_dpy, _field[i][j][_lvl]->gw());
	_lvl--;
	XClearWindow(_dpy, _lvls);
	txt(_lvls, _lvl);
	for (int i = 0; i < _x; i++)
		for (int j = 0; j < _y; j++)
		{
			XMapWindow(_dpy, _field[i][j][_lvl]->gw());
			XRaiseWindow(_dpy, _field[i][j][_lvl]->gw());
		}
	return 0;
}

void Game::light(XEvent& event)
{
	if (event.xbutton.window == _reset || event.xbutton.window == _exit)
		return;
	getcor(event);
	int x = _curcor.first;
	int y = _curcor.second;
	if (_field[x][y][_lvl]->isopen())
	{
		rlight(event);
		return;
	}
	if (_field[x][y][_lvl]->gf())
		return;
	XSetWindowBackground(_dpy, _field[x][y][_lvl]->gw(), 0xbabdbc);
	XClearWindow(_dpy, _field[x][y][_lvl]->gw());
}

void Game::rlight(XEvent& event)
{
	if (event.xbutton.window == _reset || event.xbutton.window == _exit)
		return;
	getcor(event);
	int x = _curcor.first;
	int y = _curcor.second;
	if (_field[x][y][_lvl]->gf())
		return;
	int fx = 0;
	for (int j = -1; j < 2; j++){
		for (int k = -1; k < 2; k++)
			for (int n = -1; n < 2; n++)
			{
				int nx = x + k;
				int ny = y + n;
				int nz = _lvl + j;
				if (nx >= 0 && ny >= 0 && nz >= 0 && nx < _x && ny < _y && nz < _z)
				{
					if (_field[nx][ny][_lvl]->gf() && j == 0)
						continue;
					if (!_field[nx][ny][nz]->isopen())
						XFillArc(_dpy, _field[nx][ny][_lvl]->gw(), _gc, fx, 0, 5, 5, 0, 64*360);
				}
			}
			XSetForeground(_dpy, _gc, 0x000000 + j * 1000);
			fx += 10;
		}
	XSetForeground(_dpy, _gc, 0xFF0000);
	return;
}

int Game::input(XEvent& ev)
{
	switch(ev.xbutton.button)
	{
		case 1:
			return left(ev);
		case 2:
			return mouse(ev);
		case 3:
			return right(ev);
		case 4:
			return up();
		case 5:
			return down();
		default: return 0;
	}
}

void Game::pulse(Window w, void (Game::*func) ())
{
	XSetWindowBackground(_dpy, w, 0x969069);
	XClearWindow(_dpy, w);
	(this->*func)();
}

void Game::pulse(Window w, void (Game::*func) (), unsigned long col)
{
	XSetWindowBackground(_dpy, w, col);
	XClearWindow(_dpy, w);
	(this->*func)();
}

void Game::start()
{
	int done = 0;        
	int started = 1;          
	XEvent event;       
	while(done == 0) 
	{                
		XNextEvent(_dpy, &event);      
		switch(event.type) 
		{          
			case Expose:
				expo();
				break;
			case ButtonPress:
				if (_gmvr)
					break;
				if (event.xbutton.button == 1)
					light(event);
				else if (event.xbutton.button == 2)
					rlight(event);
				break;
			case ButtonRelease:
				done = input(event);
				break;
			case EnterNotify:
				if (event.xcrossing.window == _reset)
					pulse(event.xcrossing.window, &Game::draw, 0xFF0000);
				else
					pulse(event.xcrossing.window, &Game::draw2, 0x18b4e7);
				break;
			case LeaveNotify:
				if (event.xcrossing.window == _reset)
					pulse(event.xcrossing.window, &Game::draw);
				else
					pulse(event.xcrossing.window, &Game::draw2);
				break;
			case KeyPress: 
				done = rekey(event);   
				break;
			default: break;
		}
	}   
	return;
}
