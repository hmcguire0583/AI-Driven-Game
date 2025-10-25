CC = g++
CFLAGS = --std=c++17 -Wall -Werror -pedantic -g
LIB = -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
DEPS = AIGame.hpp
SOURCES = main.cpp AIGame.cpp
OBJECTS = $(SOURCES:.cpp=.o)
STATIC_LIBRARY = AIGame.a
PROGRAM = AIGame

.PHONY: all clean lint

all: $(PROGRAM) $(STATIC_LIBRARY)

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $<

$(STATIC_LIBRARY): AIGame.o
	ar rcs $@ $^

$(PROGRAM): main.o $(STATIC_LIBRARY) $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

clean:
	rm -f *.o $(PROGRAM) $(STATIC_LIBRARY)

lint:
	cpplint *.cpp *.hpp

	