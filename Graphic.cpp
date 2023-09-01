#include <iostream>
#include <sstream>
#include <X11/Xutil.h>
#include "Cell.hpp"
#include "Input.hpp"

Menu::Menu(): _width(320), _height(240), _mode(0)
{    
	_dpy = XOpenDisplay(NULL);                				
	_scr = DefaultScreen(_dpy);       
	_depth = DefaultDepth(_dpy, _scr);	
	_cmap = DefaultColormap(_dpy, _scr);
	_maxWidth = DisplayWidth(_dpy, _scr);
	_maxHeight = DisplayHeight(_dpy, _scr);
	_show[0] = &Menu::show1;
	_show[1] = &Menu::show2;
}

void Menu::txt(Window w, const char* s)
{
	XWindowAttributes xwa;
	XGetWindowAttributes(_dpy, w, &xwa);
	std::ostringstream os;
	os << s;
	std::string conv = os.str();
	XDrawString(_dpy, w, _gc, (xwa.width-conv.size()*9) / 2, xwa.height / 2, conv.c_str(), conv.size());
}

void Menu::txt(Window w, int x, int y, const char* s)
{
	std::ostringstream os;
	os << s;
	std::string conv = os.str();
	XDrawString(_dpy, w, _gc, x, y, conv.c_str(), conv.size());
}

void Menu::create()
{
	int space = 32;
	int x = 0;
	int w = _width / 2;
	int h = _height / 4;
	int y = space;
	_height += _height/4;
	XSetWindowAttributes attr;      
	attr.override_redirect = False;
	attr.background_pixel = 0x689782;
	attr.event_mask = (KeyPressMask | ExposureMask);	       				                  			  				
	_win = XCreateWindow(_dpy, DefaultRootWindow(_dpy), (_maxWidth - _width) / 2, (_maxHeight - _height) / 2, _width, _height, 4, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	_gc = XCreateGC(_dpy, _win, 0, 0);
	if((_fnptr = XLoadQueryFont(_dpy, "9x15")) != NULL)
		XSetFont(_dpy, _gc, _fnptr->fid);
	XSizeHints hint;  
	hint.flags = (PMinSize | PMaxSize | PPosition);
	hint.min_width = hint.max_width = _width;
	hint.min_height = hint.max_height = _height;
	XSetNormalHints(_dpy, _win, &hint);
	XStoreName(_dpy, _win, "MineSweeper");
	attr.background_pixel = 0x969069;
	attr.override_redirect = True;
	attr.event_mask = (ButtonPressMask | ButtonReleaseMask | ExposureMask | EnterWindowMask | LeaveWindowMask);
	_diffs[0] = XCreateWindow(_dpy, _win, x, y, w, h, 2, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	_diffs[1] = XCreateWindow(_dpy, _win, x, h+space, w, h, 2, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	_diffs[2] = XCreateWindow(_dpy, _win, x, 2*h+space, w, h, 2, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	_diffs[4] = XCreateWindow(_dpy, _win, x, 3*h+space, w, h, 2, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	_diffs[3] = XCreateWindow(_dpy, _win, _width - 48, h+space+32, 32, 32, 2, _depth, InputOutput, CopyFromParent, (CWEventMask | CWOverrideRedirect | CWBackPixel),  &attr);
	XMapWindow(_dpy, _win);
	XMapSubwindows(_dpy, _win);
}

void Menu::show1()
{
	XMapWindow(_dpy, _win);
	XMapSubwindows(_dpy, _win);
	txt(_win, 10, 20, "MineSweeper 3D!");
	txt(_diffs[0], "Easy");
	txt(_diffs[1], "Medium");
	txt(_diffs[2], "Hard");
	txt(_diffs[4], "Custom");
	txt(_win, _width - 64, _height / 4+100, "Classic");
	txt(_diffs[0], 85, 55, "10 Mines");
	txt(_diffs[1], 85, 55, "33 Mines");
	txt(_diffs[2], 85, 55, "99 Mines");
	txt(_diffs[0], 0, 55, "5x5x5");
	txt(_diffs[1], 0, 55, "7x7x7");
	txt(_diffs[2], 0, 55, "10x10x10");
	txt(_diffs[4], 0, 55, "Smth special");
}

void Menu::show2()
{
	XMapWindow(_dpy, _win);
	XMapSubwindows(_dpy, _win);
	txt(_win, 10, 20, "MineSweeper 2D");
	txt(_diffs[0], "Easy");
	txt(_diffs[1], "Medium");
	txt(_diffs[2], "Hard");
	txt(_diffs[4], "Custom");
	txt(_win, _width - 64, _height / 4+100, "Make 3D");
	txt(_diffs[0], 85, 55, "10 Mines");
	txt(_diffs[1], 85, 55, "40 Mines");
	txt(_diffs[2], 85, 55, "99 Mines");
	txt(_diffs[0], 0, 55, "8x8");
	txt(_diffs[1], 0, 55, "16x16");
	txt(_diffs[2], 0, 55, "30x16");
	txt(_diffs[4], 0, 55, "Smth special");
}

int Menu::rekey(XEvent& ev)
{
	KeySym ks = XLookupKeysym(&(ev.xkey), 0);
	if (ks == XK_Escape) 	
		return 'E';
	return(0);        							
}

void Menu::start(int a, int b, int c, int d)
{
	XUnmapWindow(_dpy, _win);
	XFlush(_dpy);
	Game g(a, b, c, d);
	g.start();
	(this->*_show[_mode])();
}

void Menu::open()
{
	create(); 
	int done = 0;               
	XEvent event;      
	while(done == 0) 
	{                
		XNextEvent(_dpy, &event);      
		switch(event.type) 
		{          
			case Expose:
				(this->*_show[_mode])();
				break;
			case ButtonRelease:
				if (event.xbutton.button == 1)
				{
					if (event.xbutton.window == _diffs[0])
					{
						if (_mode)
							start(8, 8, 1, 10);
						else
							start(5, 5, 5, 10);
					}
					else if (event.xbutton.window == _diffs[1])
					{
						if (_mode)
							start(16, 16, 1, 40);					
						else
							start(7, 7, 7, 33);
					}
					else if (event.xbutton.window == _diffs[2])
					{
						if (_mode)
							start(30, 16, 1, 99);
						else
							start(10, 10, 10, 99);
					}
					else if (event.xbutton.window == _diffs[3])
					{
						_mode = (_mode + 1) % 2;
						for (int i = 0; i < 4; i++)
							XClearWindow(_dpy, _diffs[i]);
						XClearWindow(_dpy, _win);
						(this->*_show[_mode])();
					}
					else if (event.xbutton.window == _diffs[4])
					{
						XUnmapWindow(_dpy, _win);
						XFlush(_dpy);
						Input();
						XMapWindow(_dpy, _win);
					}
				}
				break;
			case EnterNotify:
				if (event.xcrossing.window == _diffs[3])
				{
					XSetWindowBackground(_dpy, event.xcrossing.window, 0x00ffc8);
					XClearWindow(_dpy, event.xcrossing.window);
					break;
				}
				XSetWindowBackground(_dpy, event.xcrossing.window, 0xb44ab5);
				XClearWindow(_dpy, event.xcrossing.window);
				(this->*_show[_mode])();
				break;
			case LeaveNotify:
				XSetWindowBackground(_dpy, event.xcrossing.window, 0x969069);
				XClearWindow(_dpy, event.xcrossing.window);
				(this->*_show[_mode])();
				break;
			case KeyPress: 
				done = rekey(event);   
				break;
			default: break;
		}
	}   
	return;
}

Menu::~Menu()
{
	for (int i = 0; i < 4; i++)
		XDestroyWindow(_dpy, _diffs[i]);
	XFreeFont(_dpy, _fnptr);
	XFreeGC(_dpy, _gc);
	XDestroyWindow(_dpy, _win);
	XCloseDisplay(_dpy);
}
