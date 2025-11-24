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
