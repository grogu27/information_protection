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
