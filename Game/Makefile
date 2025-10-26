# Compiler and flags
CC = g++
CFLAGS = -std=c++17 -Wall -Werror -pedantic -g

# SFML and filesystem libraries
LIBS = -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system -lstdc++fs

# Source and header files
DEPS = AIGame.hpp
SOURCES = main.cpp AIGame.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Output files
STATIC_LIBRARY = AIGame.a
PROGRAM = AIGame

.PHONY: all clean lint

# Default build
all: $(PROGRAM)

# Compile object files
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# Create static library
$(STATIC_LIBRARY): AIGame.o
	ar rcs $@ $^

# Link final executable
$(PROGRAM): main.o $(STATIC_LIBRARY)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Clean build artifacts
clean:
	rm -f *.o $(PROGRAM) $(STATIC_LIBRARY)

# Run cpplint if installed
lint:
	cpplint *.cpp *.hpp || echo "cpplint not found"

