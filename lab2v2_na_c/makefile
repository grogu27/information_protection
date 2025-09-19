CXX = gcc
CXXFLAGS = -Wall 

LIB_NAME = libcrypto_c.a
SRC_DIR = src
LIB_DIR = lib
OBJS = $(SRC_DIR)/my_crypto.o

all: $(LIB_DIR)/$(LIB_NAME) main

$(LIB_DIR)/$(LIB_NAME): $(OBJS)
	ar rcs $@ $^

$(SRC_DIR)/my_crypto.o: $(SRC_DIR)/my_crypto.c $(SRC_DIR)/my_crypto.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

main: main.c $(LIB_DIR)/$(LIB_NAME)
	$(CXX) $(CXXFLAGS) main.c -L$(LIB_DIR) -lcrypto_c -lm -o main

test_mod_pow: test_mod_pow.c $(LIB_DIR)/$(LIB_NAME)
	$(CXX) $(CXXFLAGS) test_mod_pow.c -L$(LIB_DIR) -lcrypto_c -o test_mod_pow

clean:
	rm -f $(SRC_DIR)/*.o $(LIB_DIR)/*.a main
