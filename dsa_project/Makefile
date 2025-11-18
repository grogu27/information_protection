CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
LDFLAGS = -lssl -lcrypto

SRC = src/main.cpp src/crypto.cpp src/utils.cpp
OBJ = $(SRC:.cpp=.o)
BIN = bin/fips186-dsa

all: dirs $(BIN)

dirs:
	mkdir -p bin
	mkdir -p keys

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)
	rm -f $(BIN)

distclean: clean
	rm -rf keys
	rm -rf bin

# ================================
# Дополнительные команды проверки
# ================================

# Генерация ключей DSA
genkeys: dirs
	$(BIN) genkey -o keys/ -b 2048
	@echo "Ключи созданы в keys/"

# Подпись файла
# Использование:
#   make sign FILE=some.bin
sign:
ifndef FILE
	$(error Укажи файл: make sign FILE=имяфайла)
endif
	$(BIN) sign -k keys/dsa_private.pem -i $(FILE) -o $(FILE).sig
	@echo "Подпись создана: $(FILE).sig"

# Проверка подписи
# Использование:
#   make verify FILE=some.bin
verify:
ifndef FILE
	$(error Укажи файл: make verify FILE=имяфайла)
endif
	$(BIN) verify -k keys/dsa_public.pem -i $(FILE) -s $(FILE).sig

	@echo "Проверка завершена."
