#include <iostream>
#include <cstdlib>
#include <ctime>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./client <login>\n";
        return 1;
    }

    std::string login = argv[1];
    srand(time(0));

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect"); return 1;
    }

    // Отправляем логин
    send(sock, login.c_str(), login.size(), 0);

    char buffer[1024] = {0};
    ssize_t bytes = read(sock, buffer, sizeof(buffer));
    if (bytes <= 0) { std::cerr << "Failed to receive n\n"; close(sock); return 1; }
    int n = atoi(buffer);
    std::cout << "Received n: " << n << "\n";

    memset(buffer, 0, sizeof(buffer));
    bytes = read(sock, buffer, sizeof(buffer));
    if (bytes <= 0) { std::cerr << "Failed to receive k\n"; close(sock); return 1; }
    int k = atoi(buffer);
    std::cout << "Number of rounds: " << k << "\n";

    // Секрет клиента 
    int s = 7;
    //int s = 2;

    for (int i = 0; i < k; ++i) {
        int r = 1 + rand() % (n - 1);
        int x = (r * r) % n;

        // Отправляем x серверу
        std::string x_msg = std::to_string(x);
        send(sock, x_msg.c_str(), x_msg.size(), 0);

        // Получаем challenge e
        memset(buffer, 0, sizeof(buffer));
        bytes = read(sock, buffer, sizeof(buffer));
        if (bytes <= 0) { std::cerr << "No challenge received\n"; break; }
        int e = atoi(buffer);
        std::cout << "e = " << e << "\n";

        int y = (e == 0) ? r : (r * s) % n;

        // Отправляем y серверу
        std::string y_msg = std::to_string(y);
        send(sock, y_msg.c_str(), y_msg.size(), 0);

        // Получаем результат раунда
        memset(buffer, 0, sizeof(buffer));
        bytes = read(sock, buffer, sizeof(buffer));
        if (bytes <= 0) { std::cerr << "No result from server\n"; break; }
        std::string round_result(buffer, bytes);
        std::cout << "Round " << i + 1 << " result: " << round_result << "\n";

        if (round_result == "FAIL") {
            std::cout << "Authentication failed!\n";
            close(sock);
            return 0;
        }
    }

    // Финальный результат
    memset(buffer, 0, sizeof(buffer));
    bytes = read(sock, buffer, sizeof(buffer));
    if (bytes > 0) {
        std::string final_result(buffer, bytes);
        if (final_result == "SUCCESS")
            std::cout << "Authentication successful!\n";
        else
            std::cout << "Authentication failed!\n";
    }

    close(sock);
    return 0;
}
