CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall
SFMLFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lboost_system

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))
TARGET = poker_gui

all: $(TARGET)

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFMLFLAGS)

clean:
	rm -rf build/*.o $(TARGET)

run: all
	./$(TARGET)