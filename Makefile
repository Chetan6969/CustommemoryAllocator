# ======= Makefile =======
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = allocator
OBJS = main.o allocator.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp allocator.h
	$(CXX) $(CXXFLAGS) -c main.cpp

allocator.o: allocator.cpp allocator.h
	$(CXX) $(CXXFLAGS) -c allocator.cpp

clean:
	rm -f $(OBJS) $(TARGET)

