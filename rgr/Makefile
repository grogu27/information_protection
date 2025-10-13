CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
GRAPHS_DIR = graphs

# Исполняемые файлы
ZKP_BIN = zkp_coloring
GEN_BIN = generate_graph

.PHONY: all clean run1 run2 gen1 gen2

all: $(ZKP_BIN) $(GEN_BIN)

$(ZKP_BIN): src/main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(GEN_BIN): src/generate_graph.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(ZKP_BIN) $(GEN_BIN)

run1: all
	./$(ZKP_BIN) $(GRAPHS_DIR)/graph1.txt

run2: all
	./$(ZKP_BIN) $(GRAPHS_DIR)/graph2.txt

# Генерация тестовых графов
gen1: $(GEN_BIN)
	./$(GEN_BIN) 10 15 $(GRAPHS_DIR)/graph1.txt

gen2: $(GEN_BIN)
	./$(GEN_BIN) 20 40 $(GRAPHS_DIR)/graph2.txt

help:
	@echo "Цели:"
	@echo "  all     — собрать всё"
	@echo "  clean   — удалить бинарники"
	@echo "  run1/run2 — запустить ZKP"
	@echo "  gen1/gen2 — сгенерировать графы"