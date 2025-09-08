# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
AR = ar
ARFLAGS = rcs

# Имена файлов
LIB_NAME = libcrypto
STATIC_LIB = $(LIB_NAME).a
SHARED_LIB = $(LIB_NAME).so
MAIN_EXEC = crypto_program

# Исходные файлы
SOURCES = crypto_lib.cpp
OBJECTS = $(SOURCES:.cpp=.o)
MAIN_SOURCE = main.cpp
MAIN_OBJECT = $(MAIN_SOURCE:.cpp=.o)

# Директории
INC_DIR = .
LIB_DIR = .

# Цели по умолчанию
.PHONY: all clean static shared program test install

all: static program

# Сборка статической библиотеки
static: $(STATIC_LIB)

$(STATIC_LIB): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^
	@echo "Создана статическая библиотека: $@"

# Сборка динамической библиотеки
shared: $(SHARED_LIB)

$(SHARED_LIB):
	$(CXX) $(CXXFLAGS) -shared -fPIC $(SOURCES) -o $@
	@echo "Создана динамическая библиотека: $@"

# Компиляция объектных файлов
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Сборка основной программы с использованием статической библиотеки
program: $(MAIN_EXEC)

$(MAIN_EXEC): $(MAIN_SOURCE) $(STATIC_LIB)
	$(CXX) $(CXXFLAGS) $(MAIN_SOURCE) -L. -lcrypto -o $@
	@echo "Создана программа: $@"

# Альтернативная сборка программы с динамической библиотекой
program-shared: $(MAIN_SOURCE) $(SHARED_LIB)
	$(CXX) $(CXXFLAGS) $(MAIN_SOURCE) -L. -lcrypto -o $(MAIN_EXEC)-shared
	@echo "Создана программа с динамической библиотекой: $(MAIN_EXEC)-shared"

# Тестирование
test: program
	@echo "=== Запуск тестов ==="
	./$(MAIN_EXEC)

# Быстрая сборка и запуск
run: program
	./$(MAIN_EXEC)

# Очистка
clean:
	rm -f *.o $(STATIC_LIB) $(SHARED_LIB) $(MAIN_EXEC) $(MAIN_EXEC)-shared
	@echo "Очистка завершена"

# Установка (для системного использования)
install: static
	sudo cp $(STATIC_LIB) /usr/local/lib/
	sudo cp crypto_lib.h /usr/local/include/
	sudo ldconfig
	@echo "Библиотека установлена в систему"

# Проверка существующих файлов
check:
	@echo "=== Проверка файлов ==="
	@if [ -f "$(STATIC_LIB)" ]; then echo "✓ Статическая библиотека существует"; else echo "✗ Статическая библиотека отсутствует"; fi
	@if [ -f "$(SHARED_LIB)" ]; then echo "✓ Динамическая библиотека существует"; else echo "✗ Динамическая библиотека отсутствует"; fi
	@if [ -f "$(MAIN_EXEC)" ]; then echo "✓ Исполняемый файл существует"; else echo "✗ Исполняемый файл отсутствует"; fi

# Информация о проекте
info:
	@echo "=== Информация о проекте ==="
	@echo "Статическая библиотека: $(STATIC_LIB)"
	@echo "Динамическая библиотека: $(SHARED_LIB)"
	@echo "Исполняемый файл: $(MAIN_EXEC)"
	@echo "Исходные файлы: $(SOURCES) $(MAIN_SOURCE)"
	@echo "Компилятор: $(CXX)"
	@echo "Флаги: $(CXXFLAGS)"