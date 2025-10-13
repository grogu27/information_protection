CXX = g++
CXXFLAGS = -Wall -std=c++17

# Было: LIB_NAME = libcrypto.a
LIB_NAME = libmycrypto.a  # ← изменено!

SRC_DIR = src
LIB_DIR = lib
OBJS = $(SRC_DIR)/crypto.o

all: $(LIB_DIR)/$(LIB_NAME) main

$(LIB_DIR)/$(LIB_NAME): $(OBJS)
	mkdir -p $(LIB_DIR)
	ar rcs $@ $^

$(SRC_DIR)/crypto.o: $(SRC_DIR)/crypto.cpp $(SRC_DIR)/crypto.hpp
	mkdir -p $(SRC_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

main: main.cpp $(LIB_DIR)/$(LIB_NAME)
	$(CXX) $(CXXFLAGS) main.cpp -L$(LIB_DIR) -lmycrypto -lssl -lcrypto -o main

test_mod_pow: test_mod_pow.cpp $(LIB_DIR)/$(LIB_NAME)
	$(CXX) $(CXXFLAGS) test_mod_pow.cpp -L$(LIB_DIR) -lmycrypto -lssl -lcrypto -o test_mod_pow

clean:
	rm -rf $(SRC_DIR)/*.o $(LIB_DIR)/*.a main test_mod_pow