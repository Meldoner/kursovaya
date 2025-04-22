#include <iostream>
#include <vector>
#include <cmath>
#include <windows.h>  // Для функций SetConsoleCP и SetConsoleOutputCP
#include <chrono>     // Для измерения времени
#include <locale>     // Для setlocale
#include <limits>     // Для numeric_limits (в исправлении cin.ignore)
#include <ctime>      // для time()
#define _USE_MATH_DEFINES
#include <cmath>      // для cos(), sin(), M_PI

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std; // Используем пространство имен std

// Структура для представления точки на плоскости
struct Point {
    double x, y;
};

// Функция генерации многоугольника с заданным числом вершин
vector<Point> generateRandomPolygon(int numVertices, double radius = 100.0, double irregularity = 0.3) {
    if (numVertices < 3) return {}; // Многоугольник должен иметь не менее 3 вершин
    
    vector<Point> vertices(numVertices);
    
    // Используем текущее время как seed для генератора случайных чисел, если еще не инициализирован
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(nullptr)));
        seeded = true;
    }
    
    // Генерируем углы для правильного многоугольника и добавляем немного случайности
    double angleStep = 2 * M_PI / numVertices;
    
    for (int i = 0; i < numVertices; i++) {
        // Базовый угол для текущей вершины
        double angle = i * angleStep;
        
        // Добавляем немного случайности к углу (до +/- irregularity * angleStep)
        double angleVariation = (static_cast<double>(rand()) / RAND_MAX * 2 - 1) * irregularity * angleStep;
        angle += angleVariation;
        
        // Варьируем радиус для создания более неправильной формы
        double radiusVariation = 1.0 + (static_cast<double>(rand()) / RAND_MAX * 2 - 1) * irregularity;
        double currentRadius = radius * radiusVariation;
        
        // Вычисляем координаты вершины
        vertices[i].x = currentRadius * cos(angle);
        vertices[i].y = currentRadius * sin(angle);
        //cout << vertices[i].x << " " << vertices[i].y << endl;
    }
    
    // Смещаем в случайную позицию в пределах ±radius по обеим осям
    double offsetX = (static_cast<double>(rand()) / RAND_MAX * 2 - 1) * radius;
    double offsetY = (static_cast<double>(rand()) / RAND_MAX * 2 - 1) * radius;
    
    for (auto& p : vertices) {
        p.x += offsetX;
        p.y += offsetY;
    }
    
    return vertices;
}

// --- Алгоритм 1: Формула Гаусса (шнурка) ---

// Функция для вычисления площади многоугольника по формуле Гаусса
double calculatePolygonArea_Shoelace(const vector<Point>& vertices) {
    int n = vertices.size();
    if (n < 3) return 0; // Многоугольник должен иметь минимум 3 вершины

    double area = 0.0;

    // Применяем формулу Гаусса (формулу шнурка)
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n; // Следующая вершина с замыканием
        area += vertices[i].x * vertices[j].y;
        area -= vertices[j].x * vertices[i].y;
    }

    // Берем абсолютное значение и делим на 2
    // Используем std::abs для double
    return abs(area) / 2.0;
}

// --- Алгоритм 2: Триангуляция (Исправленная версия) ---

// Функция для вычисления ЗНАКОВОЙ площади треугольника по координатам вершин
// Убрали std::abs, чтобы получить знаковую площадь
double signedTriangleArea(const Point& a, const Point& b, const Point& c) {
    return 0.5 * (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y));
}

// Функция для вычисления площади многоугольника через триангуляцию
// (Исправленная версия, работает для простых многоугольников)
double calculatePolygonArea_Triangulation(const vector<Point>& vertices) {
    int n = vertices.size();
    if (n < 3) return 0; // Многоугольник должен иметь минимум 3 вершины

    // Выбираем первую вершину как опорную точку
    Point anchor = vertices[0];

    double totalSignedArea = 0.0; // Переменная для суммирования знаковых площадей

    // Разбиваем многоугольник на треугольники с опорной точкой
    // Треугольники: (вершина 0, вершина i, вершина i+1)
    for (int i = 1; i < n - 1; i++) {
        // Суммируем знаковые площади треугольников
        totalSignedArea += signedTriangleArea(anchor, vertices[i], vertices[i + 1]);
    }

    // Возвращаем абсолютное значение итоговой знаковой площади
    // Используем std::abs для double
    return std::abs(totalSignedArea);
}


int main() {
    // Настройка поддержки русского языка в консоли
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    int n, type;
    cout << "Введите количество вершин многоугольника: ";
    cin >> n;

    if (n < 3) {
        cout << "Ошибка: многоугольник должен иметь минимум 3 вершины." << endl;
        return 1;
    }

    cout << "Введите тип ввода (1 - случайный, 2 - ручной): ";
    cin >> type;
    vector<Point> vertices(n);
    if (type == 2) {
        cout << "Введите координаты вершин (x y) в порядке обхода по периметру:" << endl;
        for (int i = 0; i < n; i++) {
            cout << "Вершина " << i + 1 << ": ";
            cin >> vertices[i].x >> vertices[i].y;
    }
    } else if (type == 1) {
        vertices = generateRandomPolygon(n);
    } else {
        cout << "Ошибка: неверный тип ввода." << endl;
        return 1;
    }
    

    // --- Измерение времени для формулы Гаусса ---
    auto start_shoelace = chrono::high_resolution_clock::now();
    double area_shoelace = calculatePolygonArea_Shoelace(vertices);
    auto end_shoelace = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration_shoelace = end_shoelace - start_shoelace;

    cout << "\n--- Формула Гаусса (шнурка) ---" << endl;
    cout << "Площадь многоугольника: " << area_shoelace << endl;
    cout << "Время выполнения: " << duration_shoelace.count() << " мс" << endl;

    // --- Измерение времени для триангуляции ---
    auto start_triangulation = chrono::high_resolution_clock::now();
    double area_triangulation = calculatePolygonArea_Triangulation(vertices);
    auto end_triangulation = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration_triangulation = end_triangulation - start_triangulation;

    cout << "\n--- Метод триангуляции ---" << endl;
    cout << "Площадь многоугольника: " << area_triangulation << endl;
    cout << "Время выполнения: " << duration_triangulation.count() << " мс" << endl;
    // Убрали примечание о неточности, так как метод исправлен


    // Задержка перед закрытием
    cout << "\nНажмите Enter для завершения...";
    // Правильная очистка буфера ввода перед get()
    cin.ignore(); 
    cin.get();

    return 0;
}