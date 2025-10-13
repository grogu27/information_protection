CXX = g++
CXXFLAGS = -Wall -std=c++17

LIB_NAME = libcrypto.a
SRC_DIR = src
LIB_DIR = lib
OBJS = $(SRC_DIR)/crypto.o

all: $(LIB_DIR)/$(LIB_NAME) main

$(LIB_DIR)/$(LIB_NAME): $(OBJS)
	ar rcs $@ $^

$(SRC_DIR)/crypto.o: $(SRC_DIR)/crypto.cpp $(SRC_DIR)/crypto.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main: main.cpp $(LIB_DIR)/$(LIB_NAME)
	$(CXX) $(CXXFLAGS) main.cpp -L$(LIB_DIR) -lcrypto -o main

test_mod_pow: test_mod_pow.cpp $(LIB_DIR)/$(LIB_NAME)
	$(CXX) $(CXXFLAGS) test_mod_pow.cpp -L$(LIB_DIR) -lcrypto -o test_mod_pow

clean:
	rm -f $(SRC_DIR)/*.o $(LIB_DIR)/*.a main


# Сгенерированные параметры:
# p = 817933
# g = 2
# x (секретный) = 149764
# y (открытый) = 111302
# sha256sum makefile makefile_dec
