#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

struct User {
    std::string login;
    int n;
    int v;
};

User find_user(const std::string& login) {
    std::ifstream file("users.txt");
    std::string l;
    int n, v;
    while (file >> l >> n >> v) {
        if (l == login) return {l, n, v};
    }
    return {"", 0, 0};
}

int main() {
    srand(time(0));

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) { perror("socket"); exit(1); }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(12345);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) { perror("bind"); exit(1); }
    if (listen(server_fd, 5) < 0) { perror("listen"); exit(1); }

    std::cout << "Server listening on port 12345...\n";

    while (true) {
        sockaddr_in client_addr{};
        socklen_t addrlen = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
        if (client_fd < 0) { perror("accept"); continue; }

        char buffer[1024] = {0};
        ssize_t bytes = read(client_fd, buffer, sizeof(buffer));
        if (bytes <= 0) { close(client_fd); continue; }

        std::string login(buffer, bytes);
        User u = find_user(login);
        if (u.login.empty()) {
            std::string msg = "ERROR: User not found";
            send(client_fd, msg.c_str(), msg.size(), 0);
            std::cout << "User not found: " << login << "\n";
            close(client_fd);
            continue;
        }

        // отправляем n
        std::string n_msg = std::to_string(u.n);
        send(client_fd, n_msg.c_str(), n_msg.size(), 0);

        int k = 3; // количество проверок
        std::string k_msg = std::to_string(k);
        send(client_fd, k_msg.c_str(), k_msg.size(), 0);
        bool auth_fail = false;

        for (int i = 0; i < k; ++i) {
            // получаем x
            memset(buffer, 0, sizeof(buffer));
            bytes = read(client_fd, buffer, sizeof(buffer));
            if (bytes <= 0) { auth_fail = true; break; }
            int x = atoi(buffer);

            // challenge
            int e = rand() % 2;
            std::string c_msg = std::to_string(e);
            send(client_fd, c_msg.c_str(), c_msg.size(), 0);

            // получаем y
            memset(buffer, 0, sizeof(buffer));
            bytes = read(client_fd, buffer, sizeof(buffer));
            if (bytes <= 0) { auth_fail = true; break; }
            int y = atoi(buffer);

            int lhs = (y * y) % u.n;
            int rhs = (x * (e ? u.v : 1)) % u.n;

            std::string result = (lhs == rhs) ? "OK" : "FAIL";
            send(client_fd, result.c_str(), result.size(), 0);

            if (lhs != rhs) {
                //std::cout << "Authentication failed for " << u.login << "\n";
                auth_fail = true;
                break;
            }
        }

        std::string final_msg = auth_fail ? "FAIL" : "SUCCESS";
        send(client_fd, final_msg.c_str(), final_msg.size(), 0);
        std::cout << login << " authentication: " << final_msg << "\n";

        close(client_fd);
    }

    close(server_fd);
}
