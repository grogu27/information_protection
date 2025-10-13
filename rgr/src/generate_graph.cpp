/**
 * Генератор случайного графа с гарантированной корректной 3-раскраской.
 * 
 * Алгоритм:
 * 1. Задаём n — число вершин.
 * 2. Случайно назначаем каждой вершине цвет из {1,2,3}.
 * 3. Добавляем рёбра ТОЛЬКО между вершинами с разными цветами.
 * 4. Гарантируем, что граф связный (опционально — для наглядности).
 * 
 * Использование: ./generate_graph <n> <m> <output_file>
 */

#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <algorithm>
#include <set>

using namespace std;

random_device rd;
mt19937 gen(rd());

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Использование: " << argv[0] << " <n> <m> <файл_вывода>\n";
        return 1;
    }

    int n = stoi(argv[1]);
    int m = stoi(argv[2]);
    string outfile = argv[3];

    if (n < 1 || m < 0) {
        cerr << "Ошибка: n >= 1, m >= 0\n";
        return 1;
    }

    // Максимальное число рёбер между разными цветами
    // В худшем случае — полный 3-дольный граф: до ~ (2/3)*n^2 рёбер
    if (m > n * (n - 1) / 2) {
        cerr << "Предупреждение: запрошено слишком много рёбер. Уменьшаем до максимума.\n";
        m = n * (n - 1) / 2;
    }

    // Шаг 1: генерируем случайную раскраску
    vector<int> color(n);
    uniform_int_distribution<> colorDist(1, 3);
    for (int i = 0; i < n; ++i) {
        color[i] = colorDist(gen);
    }

    // Шаг 2: собираем все возможные рёбра между разными цветами
    vector<pair<int, int>> possibleEdges;
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (color[i] != color[j]) {
                possibleEdges.emplace_back(i, j);
            }
        }
    }

    if ((int)possibleEdges.size() < m) {
        cerr << "Предупреждение: невозможно создать " << m << " рёбер с текущей раскраской.\n";
        cerr << "Максимум: " << possibleEdges.size() << ". Используем максимум.\n";
        m = possibleEdges.size();
    }

    // Перемешиваем и берём первые m рёбер
    shuffle(possibleEdges.begin(), possibleEdges.end(), gen);
    possibleEdges.resize(m);

    // Шаг 3: сохраняем в файл
    ofstream out(outfile);
    if (!out.is_open()) {
        cerr << "Не удалось создать файл '" << outfile << "'\n";
        return 1;
    }

    out << n << " " << m << "\n";
    for (auto [u, v] : possibleEdges) {
        out << u + 1 << " " << v + 1 << "\n"; // 1-based
    }
    for (int i = 0; i < n; ++i) {
        out << color[i] << (i == n - 1 ? "\n" : " ");
    }
    out.close();

    cout << "Граф с " << n << " вершинами и " << m << " рёбрами сохранён в '" << outfile << "'\n";
    return 0;
}