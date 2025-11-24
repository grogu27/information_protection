CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall
SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))
TARGET = poker

all: $(TARGET)

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf build/*.o $(TARGET)

run: all
	./$(TARGET)
