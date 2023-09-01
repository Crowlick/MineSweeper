LIBS = Cell.hpp Graphic.hpp Gameplay.hpp Input.hpp

MS: main.o Gameplay.o Graphic.o Input.o Cell.o Gameplay2.o
	g++ Cell.o Gameplay.o Gameplay2.o main.o Graphic.o Input.o -lX11 -o MS

main.o: main.cpp $(LIBS)
	g++ -c main.cpp

Cell.o: Cell.cpp $(LIBS)
	g++ -c Cell.cpp

Graphic.o: Graphic.cpp $(LIBS)
	g++ -c Graphic.cpp
	
Gameplay.o: Gameplay.cpp $(LIBS)
	g++ -c Gameplay.cpp 
	
Gameplay2.o: Gameplay2.cpp $(LIBS)
	g++ -c Gameplay2.cpp
	
Input.o: Input.cpp $(LIBS)
	g++ -c Input.cpp
	
clean:
	rm *.o
