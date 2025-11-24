CXX=g++
CXXFLAGS=-std=c++17 -O2 -Wall

SRC = src/main.cpp src/graph.cpp src/zkp.cpp
OBJ = $(SRC:src/%.cpp=build/%.o)
TARGET = zkp

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

gen:
	$(CXX) $(CXXFLAGS) src/gen.cpp -o gen

# Параметры по умолчанию
N ?= 10
R ?= 10

run: all gen
	@echo "Generating test graph with n=$(N)..."
	./gen $(N)
	@echo "Running ZKP protocol for $(R) rounds..."
	./zkp graph.txt cycle.txt $(R)
clean:
	rm -rf build/*.o $(TARGET) gen

