#include <iostream>
#include <random>
#include <cmath>
#include <map>
#include <stdexcept>

using namespace std;

namespace Math {

long long mod_exp(long long base, long long exp, long long mod) {
    if (mod == 1) return 0;
    long long result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) 
            result = (result * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}

long long extended_gcd(long long a, long long b, long long &x, long long &y) {
    if (b == 0) {
        x = 1; y = 0;
        return a;
    }
    long long x1, y1;
    long long g = extended_gcd(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return g;
}

long long mod_inverse(long long a, long long m) {
    long long x, y;
    long long g = extended_gcd(a, m, x, y);
    if (g != 1) 
        throw runtime_error("modular inverse does not exist");
    return (x % m + m) % m;
}

bool is_prime(long long n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (long long i = 3; i * i <= n; i += 2)
        if (n % i == 0) return false;
    return true;
}

long long random_prime(long long min, long long max, mt19937 &rng) {
    uniform_int_distribution<long long> dist(min, max);
    long long p;
    int attempts = 0;
    do {
        p = dist(rng);
        attempts++;
        if (attempts > 1000)
            throw runtime_error("failed to generate prime");
    } while (!is_prime(p));
    return p;
}

}

struct RSAKeys {
    long long e = 0;
    long long d = 0;
    long long n = 0;

    RSAKeys() = default;
    RSAKeys(long long e_, long long d_, long long n_)
        : e(e_), d(d_), n(n_) {}
};

class VotingServer {
private:
    RSAKeys keys_;
    mt19937 rng_;

    void generate_keys() {
        long long p = Math::random_prime(53, 151, rng_);
        long long q = Math::random_prime(157, 251, rng_);
        while (p == q)
            q = Math::random_prime(157, 251, rng_);

        long long n = p * q;
        long long phi = (p - 1) * (q - 1);

        long long e = 65537;
        if (e >= phi) e = 3;

        long long x, y;
        while (Math::extended_gcd(e, phi, x, y) != 1) {
            e += 2;
            if (e >= phi) {
                e = 3;
                break;
            }
        }

        long long d = Math::mod_inverse(e, phi);
        keys_ = RSAKeys(e, d, n);

        cout << "\n===== SERVER: KEY GENERATION =====\n";
        cout << "p = " << p << "\n";
        cout << "q = " << q << "\n";
        cout << "n = p * q = " << n << "\n";
        cout << "phi(n) = (p-1)*(q-1) = " << phi << "\n";
        cout << "e = " << e << "\n";
        cout << "d = " << d << "\n";
        cout << "==================================\n";
    }

public:
    VotingServer() : rng_(random_device{}()) {
        generate_keys();
    }

    const RSAKeys& get_public_key() const { return keys_; }

    long long sign_blinded(long long blinded_msg) const {
        cout << "\n===== SERVER: SIGNING BLINDED MESSAGE =====\n";
        cout << "Blinded message received: " << blinded_msg << "\n";
        long long sig = Math::mod_exp(blinded_msg, keys_.d, keys_.n);
        cout << "Signature (blinded_msg^d mod n): " << sig << "\n";
        cout << "============================================\n";
        return sig;
    }

    bool verify(long long message, long long signature) const {
        cout << "\n===== SERVER: SIGNATURE VERIFICATION =====\n";
        cout << "Message: " << message << "\n";
        cout << "Signature: " << signature << "\n";

        long long recovered = Math::mod_exp(signature, keys_.e, keys_.n);
        cout << "Recovered (signature^e mod n): " << recovered << "\n";

        bool valid = (recovered == message);
        cout << "Verification result: " << (valid ? "SUCCESS" : "FAILURE") << "\n";
        cout << "===========================================\n";
        return valid;
    }
};

class VoterClient {
private:
    mt19937 rng_;
    long long r_ = 0;

public:
    VoterClient() : rng_(random_device{}()) {}

    long long choose_vote() {
        cout << "\n===== CLIENT: VOTE SELECTION =====\n";
        cout << "Options:\n";
        cout << "1. Yes\n";
        cout << "2. No\n";
        cout << "3. Abstain\n";
        cout << "Your choice (1-3): ";

        int choice;
        cin >> choice;

        map<int, pair<string, long long>> options = {
            {1, {"Yes", 11111}},
            {2, {"No", 22222}},
            {3, {"Abstain", 33333}}
        };

        string label = "Abstain";
        long long msg = 33333;
        if (options.find(choice) != options.end()) {
            label = options[choice].first;
            msg = options[choice].second;
        } else {
            cout << "Invalid choice. Defaulting to 'Abstain'.\n";
        }

        cout << "Selected: " << label << "\n";
        cout << "Message code: " << msg << "\n";
        cout << "==================================\n";
        return msg;
    }

    long long blind_message(long long msg, const RSAKeys& pub) {
        cout << "\n===== CLIENT: BLINDING MESSAGE =====\n";

        uniform_int_distribution<long long> dist(2, pub.n - 2);
        long long r;
        long long x, y;
        do {
            r = dist(rng_);
        } while (Math::extended_gcd(r, pub.n, x, y) != 1);

        r_ = r;
        long long r_e = Math::mod_exp(r, pub.e, pub.n);
        long long blinded = (msg * r_e) % pub.n;

        cout << "Random r: " << r << "\n";
        cout << "r^e mod n: " << r_e << "\n";
        cout << "Blinded message: " << blinded << "\n";
        cout << "====================================\n";
        return blinded;
    }

    long long unblind_signature(long long blinded_sig, const RSAKeys& pub) {
        cout << "\n===== CLIENT: UNBLINDING SIGNATURE =====\n";
        long long r_inv = Math::mod_inverse(r_, pub.n);
        long long sig = (blinded_sig * r_inv) % pub.n;
        if (sig < 0) sig += pub.n;

        cout << "r^(-1) mod n: " << r_inv << "\n";
        cout << "Final signature: " << sig << "\n";
        cout << "========================================\n";
        return sig;
    }
};

int main() {
    cout << "Lab 13: Blind Signature Protocol for Anonymous Voting\n";
    cout << "=======================================================\n";

    try {
        VotingServer server;
        VoterClient client;

        long long message = client.choose_vote();
        const RSAKeys& pub = server.get_public_key();

        long long blinded_msg = client.blind_message(message, pub);
        long long blinded_sig = server.sign_blinded(blinded_msg);
        long long final_sig = client.unblind_signature(blinded_sig, pub);

        cout << "\n===== SUBMITTING BALLOT =====\n";
        cout << "Message: " << message << "\n";
        cout << "Signature: " << final_sig << "\n";
        cout << "=============================\n";

        bool valid = server.verify(message, final_sig);

        cout << "\n===== RESULT =====\n";
        if (valid) {
            cout << "Vote accepted. Anonymity preserved.\n";
        } else {
            cout << "Vote rejected due to signature verification failure.\n";
        }
        cout << "==================\n";

    } catch (const exception& e) {
        cerr << "\nError: " << e.what() << endl;
        return 1;
    }

    return 0;
}