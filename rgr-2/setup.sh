#!/bin/bash
# setup.sh — создаёт структуру проекта для РГР по "Гамильтонову циклу" с ZKP

set -e

# Пункт задания: "информация о графах считывать из файла" → создаём src/ и build/
mkdir -p src build

# ---------- создаём main.cpp ----------
cat > src/main.cpp <<'EOF'
// main.cpp
// Пункт задания: "реализовать протокол доказательства с нулевым знанием для Гамильтонова цикла"
// main — точка входа, загружает граф и цикл из файлов, запускает ZKP
#include "graph.hpp"
#include "zkp.hpp"
#include <iostream>

int main(int argc, char** argv) {
    if(argc < 3){
        std::cout << "Usage: ./zkp graph.txt cycle.txt [rounds]\n";
        return 0;
    }

    std::string graphFile = argv[1];
    std::string cycleFile = argv[2];
    int rounds = (argc >= 4 ? std::stoi(argv[3]) : 10);

    Graph g;
    if(!g.load(graphFile)){
        std::cerr << "Failed to load graph\n";
        return 1;
    }

    std::vector<int> cycle;
    if(!g.loadCycle(cycleFile, cycle)){
        std::cerr << "Failed to load cycle\n";
        return 1;
    }

    ZKP zkp(g, cycle);
    zkp.run(rounds);

    return 0;
}
EOF

# ---------- graph.hpp ----------
cat > src/graph.hpp <<'EOF'
// graph.hpp
// Пункт задания: "считывать информацию о графах из файла"
#pragma once
#include <vector>
#include <string>

struct Graph {
    int n{}, m{};
    std::vector<std::vector<int>> adj;

    bool load(const std::string& path); // загружает граф
    bool loadCycle(const std::string& path, std::vector<int>& cyc); // загружает цикл
};
EOF

# ---------- graph.cpp ----------
cat > src/graph.cpp <<'EOF'
// graph.cpp
#include "graph.hpp"
#include <fstream>
#include <sstream>

// Пункт задания: "файл содержит описание графа: n, m и ребра"
bool Graph::load(const std::string& path){
    std::ifstream in(path);
    if(!in) return false;

    in >> n >> m;
    if(n<=0 || n>=1001) return false; // n < 1001, m <= n^2 не проверяем далее для простоты

    adj.assign(n, std::vector<int>(n,0));
    for(int i=0;i<m;i++){
        int u,v; in>>u>>v;
        u--,v--; // нумерация с 0
        if(u<0||v<0||u>=n||v>=n) return false;
        adj[u][v] = adj[v][u] = 1;
    }
    return true;
}

// Пункт задания: "дополнительная информация: гамильтонов цикл из отдельного файла"
bool Graph::loadCycle(const std::string& path, std::vector<int>& cyc){
    std::ifstream in(path);
    if(!in) return false;
    cyc.clear();
    int v;
    while(in >> v){
        v--;
        if(v<0 || v>=n) return false;
        cyc.push_back(v);
    }
    return cyc.size() == (size_t)n;
}
EOF

# ---------- zkp.hpp ----------
cat > src/zkp.hpp <<'EOF'
// zkp.hpp
// Пункт задания: "реализация протокола доказательства с нулевым знанием"
#pragma once
#include "graph.hpp"
#include <vector>
#include <map>

struct ZKP {
    Graph& g;
    std::vector<int> cycle;

    struct Commit {
        std::map<std::pair<int,int>, int> bits;
    };

    ZKP(Graph& gg, const std::vector<int>& cyc);

    void run(int rounds);
private:
    void randomPerm(std::vector<int>& p); // случайная перестановка
    std::vector<std::vector<int>> buildPermGraph(const std::vector<int>& p); // переставленный граф
    Commit commitGraph(const std::vector<std::vector<int>>& a); // коммит
    bool checkIsomorphism(const Commit& C, const std::vector<std::vector<int>>& a);
    bool checkCycle(const Commit& C, const std::vector<int>& p);
};
EOF

# ---------- zkp.cpp ----------
cat > src/zkp.cpp <<'EOF'
// zkp.cpp
#include "zkp.hpp"
#include <iostream>
#include <random>
#include <algorithm>

// Пункт задания: "демонстрация работы протокола"
ZKP::ZKP(Graph& gg, const std::vector<int>& cyc) : g(gg), cycle(cyc) {}

void ZKP::randomPerm(std::vector<int>& p){
    p.resize(g.n);
    for(int i=0;i<g.n;i++) p[i]=i;
    std::shuffle(p.begin(), p.end(), std::mt19937(std::random_device{}()));
}

std::vector<std::vector<int>> ZKP::buildPermGraph(const std::vector<int>& p){
    std::vector<std::vector<int>> a(g.n, std::vector<int>(g.n,0));
    for(int i=0;i<g.n;i++)
        for(int j=0;j<g.n;j++)
            a[i][j] = g.adj[p[i]][p[j]];
    return a;
}

ZKP::Commit ZKP::commitGraph(const std::vector<std::vector<int>>& a){
    Commit C;
    for(int i=0;i<g.n;i++)
        for(int j=i+1;j<g.n;j++)
            C.bits[{i,j}] = a[i][j];
    return C;
}

bool ZKP::checkIsomorphism(const Commit& C, const std::vector<std::vector<int>>& a){
    for(auto& kv : C.bits){
        auto ij = kv.first;
        int i=ij.first, j=ij.second;
        if(a[i][j] != kv.second) return false;
    }
    return true;
}

bool ZKP::checkCycle(const Commit& C, const std::vector<int>& p){
    std::vector<int> inv(g.n);
    for(int i=0;i<g.n;i++) inv[p[i]] = i;

    for(int i=0;i<g.n;i++){
        int u = inv[cycle[i]];
        int v = inv[cycle[(i+1)%g.n]];
        if(u>v) std::swap(u,v);
        if(C.bits.at({u,v}) != 1) return false;
    }
    return true;
}

void ZKP::run(int rounds){
    std::mt19937 rng(std::random_device{}());
    for(int r=1;r<=rounds;r++){
        std::cout << "\n=== Round " << r << " ===\n";
        std::vector<int> perm;
        randomPerm(perm);
        auto a = buildPermGraph(perm);
        auto C = commitGraph(a);
        int challenge = rng() % 2;
        std::cout << "Verifier challenge = " << challenge << "\n";

        if(challenge==0)
            std::cout << (checkIsomorphism(C,a) ? "OK\n" : "FAIL\n");
        else
            std::cout << (checkCycle(C,perm) ? "OK\n" : "FAIL\n");
    }
}
EOF

# ---------- генератор графа ----------
cat > src/gen.cpp <<'EOF'
// gen.cpp
// Пункт задания: "для тестирования необходимо генерировать правильные решения"
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>

int main(int argc, char** argv){
    if(argc<2){
        std::cout << "Usage: ./gen n\n";
        return 0;
    }
    int n = std::stoi(argv[1]);
    if(n<3 || n>1000) return 1;

    std::vector<int> cycle(n);
    for(int i=0;i<n;i++) cycle[i]=i+1;

    std::shuffle(cycle.begin(), cycle.end(), std::mt19937(std::random_device{}()));

    std::vector<std::pair<int,int>> edges;

    // добавляем гамильтонов цикл
    for(int i=0;i<n;i++){
        int u=cycle[i], v=cycle[(i+1)%n];
        edges.push_back({u,v});
    }

    // добавляем n случайных рёбер
    std::mt19937 rng(std::random_device{}());
    for(int i=0;i<n;i++){
        int u=rng()%n+1, v=rng()%n+1;
        if(u!=v) edges.push_back({u,v});
    }

    // записываем graph.txt
    std::ofstream g("graph.txt");
    g << n << " " << edges.size() << "\n";
    for(auto& e:edges) g << e.first << " " << e.second << "\n";

    // записываем cycle.txt
    std::ofstream c("cycle.txt");
    for(int x:cycle) c << x << " ";

    std::cout << "Generated graph.txt and cycle.txt\n";
    return 0;
}
EOF

# ---------- Makefile ----------
cat > Makefile <<'EOF'
CXX=g++
CXXFLAGS=-std=c++17 -O2 -Wall

SRC = src/main.cpp src/graph.cpp src/zkp.cpp
OBJ = $(SRC:src/%.cpp=build/%.o)
TARGET = zkp

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

gen:
	$(CXX) $(CXXFLAGS) src/gen.cpp -o gen

clean:
	rm -rf build/*.o $(TARGET) gen
EOF

echo "Setup complete! Use 'make' to build zkp, 'make gen' to build generator."
