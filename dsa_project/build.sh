#!/bin/sh
echo "Building project..."

mkdir -p bin
mkdir -p keys

g++ -std=c++17 -O2 -Wall -Wextra -c src/main.cpp -o src/main.o
g++ -std=c++17 -O2 -Wall -Wextra -c src/crypto.cpp -o src/crypto.o
g++ -std=c++17 -O2 -Wall -Wextra -c src/utils.cpp -o src/utils.o

g++ -std=c++17 -O2 -Wall -Wextra -o bin/fips186-dsa src/main.o src/crypto.o src/utils.o -lssl -lcrypto

echo "Build finished. Binary: bin/fips186-dsa"
