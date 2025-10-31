#pragma once
#include <string>
#include <vector>

void rsa_sign_file(const std::string& filename);
void rsa_verify_file(const std::string& filename);