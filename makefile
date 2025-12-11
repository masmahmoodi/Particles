CXX = g++
CXXFLAGS = -std=c++17 -O2
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

SRC = main.cpp Engine.cpp Particle.cpp Matrices.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = particles

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
