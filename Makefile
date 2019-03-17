all: DLX
DLX:dancinglinks.cpp
	g++ dancinglinks.cpp --std=c++11  -o3 -o DLX.out
clean:
	rm ./DLX.out
