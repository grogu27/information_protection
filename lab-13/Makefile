CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET   = voting_system
SRC      = voting_system.cpp

.PHONY: all clean run debug help

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lm

run: $(TARGET)
	@echo "=== Запуск (C++ версия) ==="
	./$(TARGET)

debug: CXXFLAGS += -g -DDEBUG
debug: clean all
	@echo "=== Сборка с отладкой ==="

clean:
	@echo "Очистка"
	@rm -f $(TARGET)

help:
	@echo "Команды:"
	@echo "  make      — сборка"
	@echo "  make run  — запуск"
	@echo "  make debug — отладочная сборка"
	@echo "  make clean — очистить"

.DEFAULT_GOAL := all