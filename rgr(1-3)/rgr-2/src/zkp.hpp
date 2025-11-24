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
