#pragma once
#include <vector>
#include <string>
#include <boost/multiprecision/cpp_int.hpp>

using cpp_int = boost::multiprecision::cpp_int;

class Deck {
public:
    static std::string cardToString(cpp_int card); // принимает расшифрованное число 2..53
    static cpp_int nameToValue(const std::string& name);
    static std::vector<cpp_int> getInitialDeck(); // [2, 3, ..., 53]
    
};