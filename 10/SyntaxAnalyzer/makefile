CXX = g++
CXXFLAGS += -O0 -Wall -Wextra -std=c++17 -g
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: clean all
.DEFAULT_GOAL := all

all: debug

debug: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o main 

clean:
	rm main *.o