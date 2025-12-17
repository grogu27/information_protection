#include "zkp.hpp"
#include <iostream>
#include <random>
#include <algorithm>

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
