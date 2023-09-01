#include <vector>

class Input
{
	Display* _dpy;
	int _depth;
	int _scr;
	int _width;
	int _height;
	Window _win;
	Window _reset;
	Window _dim[4];
	Window _next;
	Window _back;
	Window _start;
	Window _close;
	int _iw;
	int _ih;
	int _x;
	int _y;
	int _z;
	int _m;
	int _lvl;
	GC _gc;
	GC _art;
	int _space;
	int _tx[4];
	std::vector<int> _scrn[4];
	void (Input::*_draw[4])();
	void (Input::*_dnum[10])();
public:
	Input();
	void start();
	void txt(Window, const char*);
	void expo();
	int parse(std::vector<int>&);
	void drawX();
	void drawY();
	void drawZ();
	void drawM();
	void drawEq();
	void draw1();
	void draw2();
	void draw3();
	void draw4();
	void draw5();
	void draw6();
	void draw7();
	void draw8();
	void drawar(Window);
	void draw9();
	int go();
	void draw0();
	int slide(int);
	int rekey(XEvent&);
	~Input();
};
