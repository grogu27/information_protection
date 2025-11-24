/**
 * Реализация протокола доказательства с нулевым знанием (ZKP)
 * для задачи 3-раскраски графа.
 * 
 * Входной формат файла:
 * n m
 * u1 v1
 * ...
 * um vm
 * c1 c2 ... cn   (цвета: 1, 2 или 3)
 * 
 * Вариант: 1 (Раскраска графа)
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <algorithm>
#include <string>

using namespace std;

// Глобальный генератор случайных чисел
random_device rd;
mt19937 gen(rd());

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Использование: " << argv[0] << " <файл_графа>\n";
        return 1;
    }

    string filename = argv[1];
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка: не удалось открыть файл '" << filename << "'\n";
        return 1;
    }

    int n, m;
    file >> n >> m;

    if (n <= 0 || m <= 0) {
        cerr << "Ошибка: некорректное количество вершин или рёбер.\n";
        return 1;
    }

    vector<pair<int, int>> edges;
    for (int i = 0; i < m; ++i) {
        int u, v;
        file >> u >> v;
        // Переводим в 0-based индексацию
        edges.emplace_back(u - 1, v - 1);
    }

    vector<int> trueColoring(n);
    for (int i = 0; i < n; ++i) {
        file >> trueColoring[i];
        if (trueColoring[i] < 1 || trueColoring[i] > 3) {
            cerr << "Ошибка: цвет должен быть 1, 2 или 3 (вершина " << i + 1 << ")\n";
            return 1;
        }
    }
    file.close();

    // Проверка корректности раскраски
    bool valid = true;
    for (const auto& [u, v] : edges) {
        if (trueColoring[u] == trueColoring[v]) {
            valid = false;
            break;
        }
    }

    if (!valid) {
        cout << "Предупреждение: предоставленная раскраска НЕКОРРЕКТНА!\n";
        cout << "Протокол ZKP не может быть выполнен честно.\n";
        return 1;
    }

    cout << "Раскраска корректна. Запуск протокола доказательства с нулевым знанием...\n";
    cout << "Количество вершин: " << n << ", рёбер: " << m << "\n";
    cout << "Число раундов: 30\n\n";

    const int ROUNDS = 30;
    uniform_int_distribution<> edgeDist(0, m - 1);

    for (int round = 1; round <= ROUNDS; ++round) {
        // Шаг 1: Prover создаёт случайную перестановку цветов {1,2,3}
        vector<int> perm = {1, 2, 3};
        shuffle(perm.begin(), perm.end(), gen);

        // Применяем перестановку к раскраске
        vector<int> permuted(n);
        for (int i = 0; i < n; ++i) {
            permuted[i] = perm[trueColoring[i] - 1];
        }

        // Шаг 2: Verifier выбирает случайное ребро
        int idx = edgeDist(gen);
        int u = edges[idx].first;
        int v = edges[idx].second;

        // Шаг 3: Prover раскрывает цвета u и v
        int cu = permuted[u];
        int cv = permuted[v];

        // Шаг 4: Verifier проверяет
        if (cu == cv) {
            cout << "Раунд " << round << ": обнаружено совпадение цветов на ребре ("
                 << u + 1 << ", " << v + 1 << ") → " << cu << " = " << cv << "\n";
            cout << "Доказательство отклонено.\n";
            return 1;
        }

        // Раскомментируйте для отладки:
        // cout << "Раунд " << round << ": ребро (" << u+1 << "," << v+1
        //      << ") → цвета " << cu << " и " << cv << " — OK\n";
    }

    cout << "Протокол завершён успешно!\n";
    cout << "С высокой вероятностью Prover действительно знает корректную 3-раскраску.\n";
    return 0;
}