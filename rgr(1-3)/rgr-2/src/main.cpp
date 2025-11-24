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
