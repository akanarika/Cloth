all: main

main: main.o cloth.o point.o
	g++ main.o cloth.o point.o -o main -framework OpenGL -lGLEW -lglfw3

main.o: main.cpp
	g++ -c main.cpp

cloth.o: cloth.cpp
	g++ -c cloth.cpp

point.o: point.cpp
	g++ -c point.cpp

clean:
	rm *.o 
	rm .*.swp

