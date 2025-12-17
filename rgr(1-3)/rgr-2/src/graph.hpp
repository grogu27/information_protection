#pragma once
#include <vector>
#include <string>

struct Graph {
    int n{}, m{};
    std::vector<std::vector<int>> adj;

    bool load(const std::string& path); // загружает граф
    bool loadCycle(const std::string& path, std::vector<int>& cyc); // загружает цикл
};
