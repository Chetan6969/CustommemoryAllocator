all: allocator

allocator: main.cpp allocator.cpp
	g++ -std=c++11 main.cpp allocator.cpp -o allocator

clean:
	rm -f allocator

