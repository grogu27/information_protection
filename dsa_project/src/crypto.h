#pragma once
#include <string>
#include <vector>

bool generate_dsa_keypair(const std::string& priv_out, const std::string& pub_out, int bits = 2048);

bool sign_file_with_dsa(const std::string& priv_pem, const std::string& infile,
                        const std::string& sig_out, bool embed=false);

bool verify_file_with_dsa(const std::string& pub_pem, const std::string& infile,
                          const std::string& sig_file, bool embedded=false);
