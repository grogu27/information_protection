#pragma once
#include <string>
#include <vector>

std::vector<unsigned char> sha256_file(const std::string& filename);
void write_file(const std::string& filename, const std::vector<unsigned char>& data);
std::vector<unsigned char> read_file(const std::string& filename);