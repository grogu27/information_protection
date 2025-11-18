#!/bin/bash
# Создание структуры проекта
mkdir -p dsa_project/{src,include}
cd dsa_project

# Генерация файлов
cat > create_project.sh << 'EOF'
#!/bin/bash
mkdir -p src include
touch src/main.cpp src/dsa.cpp src/file_utils.cpp
touch include/dsa.h include/file_utils.h
touch Makefile README.md
echo "Структура проекта создана."
EOF

chmod +x create_project.sh

# Makefile
cat > Makefile << 'EOF'
CC = g++
CFLAGS = -std=c++17 -Wall -Iinclude -O2
LDFLAGS = -lcrypto -lgmp

SOURCES = src/main.cpp src/dsa.cpp src/file_utils.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = dsa_sign

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
EOF

# README.md
cat > README.md << 'EOF'
# DSA File Signer (FIPS 186)

## Требования
- OpenSSL (для SHA-256 и CSPRNG)
- GMP (арифметика больших чисел)
- GCC 9.0+

## Сборка
```bash
make