CXX = g++
CXXFLAGS = -std=c++17 -O2 -Iinclude -Wall -Wno-deprecated-declarations
LIBS = -lssl -lcrypto
SRC = src/main.cpp src/rsa_sign.cpp src/elgamal_sign.cpp src/utils.cpp
TARGET = sign_tool

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET) *.sig *.ver private.pem public.pem elgamal_*.key

.PHONY: clean



# Как использовать
# make
# ./sign_tool rsa document.txt     # создаёт document.txt_sig и document.txt_ver
# ./sign_tool elgamal image.png    # создаёт image.png_sig и image.png_ver