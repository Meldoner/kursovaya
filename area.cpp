#include <iostream>
#include <vector>
#include <cmath>
#include <windows.h>  // ��� ������� SetConsoleCP � SetConsoleOutputCP
#include <chrono>     // ��� ��������� �������
#include <locale>     // ��� setlocale
#include <limits>     // ��� numeric_limits (� ����������� cin.ignore)
#include <ctime>      // ��� time()
#define _USE_MATH_DEFINES
#include <cmath>      // ��� cos(), sin(), M_PI

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std; // ���������� ������������ ���� std

// ��������� ��� ������������� ����� �� ���������
struct Point {
    double x, y;
};

// ������� ��������� �������������� � �������� ������ ������
vector<Point> generateRandomPolygon(int numVertices, double radius = 100.0, double irregularity = 0.3) {
    if (numVertices < 3) return {}; // ������������� ������ ����� �� ����� 3 ������
    
    vector<Point> vertices(numVertices);
    
    // ���������� ������� ����� ��� seed ��� ���������� ��������� �����, ���� ��� �� ���������������
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(nullptr)));
        seeded = true;
    }
    
    // ���������� ���� ��� ����������� �������������� � ��������� ������� �����������
    double angleStep = 2 * M_PI / numVertices;
    
    for (int i = 0; i < numVertices; i++) {
        // ������� ���� ��� ������� �������
        double angle = i * angleStep;
        
        // ��������� ������� ����������� � ���� (�� +/- irregularity * angleStep)
        double angleVariation = (static_cast<double>(rand()) / RAND_MAX * 2 - 1) * irregularity * angleStep;
        angle += angleVariation;
        
        // ��������� ������ ��� �������� ����� ������������ �����
        double radiusVariation = 1.0 + (static_cast<double>(rand()) / RAND_MAX * 2 - 1) * irregularity;
        double currentRadius = radius * radiusVariation;
        
        // ��������� ���������� �������
        vertices[i].x = currentRadius * cos(angle);
        vertices[i].y = currentRadius * sin(angle);
        //cout << vertices[i].x << " " << vertices[i].y << endl;
    }
    
    // ������� � ��������� ������� � �������� �radius �� ����� ����
    double offsetX = (static_cast<double>(rand()) / RAND_MAX * 2 - 1) * radius;
    double offsetY = (static_cast<double>(rand()) / RAND_MAX * 2 - 1) * radius;
    
    for (auto& p : vertices) {
        p.x += offsetX;
        p.y += offsetY;
    }
    
    return vertices;
}

// --- �������� 1: ������� ������ (������) ---

// ������� ��� ���������� ������� �������������� �� ������� ������
double calculatePolygonArea_Shoelace(const vector<Point>& vertices) {
    int n = vertices.size();
    if (n < 3) return 0; // ������������� ������ ����� ������� 3 �������

    double area = 0.0;

    // ��������� ������� ������ (������� ������)
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n; // ��������� ������� � ����������
        area += vertices[i].x * vertices[j].y;
        area -= vertices[j].x * vertices[i].y;
    }

    // ����� ���������� �������� � ����� �� 2
    // ���������� std::abs ��� double
    return abs(area) / 2.0;
}

// --- �������� 2: ������������ (������������ ������) ---

// ������� ��� ���������� �������� ������� ������������ �� ����������� ������
// ������ std::abs, ����� �������� �������� �������
double signedTriangleArea(const Point& a, const Point& b, const Point& c) {
    return 0.5 * (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y));
}

// ������� ��� ���������� ������� �������������� ����� ������������
// (������������ ������, �������� ��� ������� ���������������)
double calculatePolygonArea_Triangulation(const vector<Point>& vertices) {
    int n = vertices.size();
    if (n < 3) return 0; // ������������� ������ ����� ������� 3 �������

    // �������� ������ ������� ��� ������� �����
    Point anchor = vertices[0];

    double totalSignedArea = 0.0; // ���������� ��� ������������ �������� ��������

    // ��������� ������������� �� ������������ � ������� ������
    // ������������: (������� 0, ������� i, ������� i+1)
    for (int i = 1; i < n - 1; i++) {
        // ��������� �������� ������� �������������
        totalSignedArea += signedTriangleArea(anchor, vertices[i], vertices[i + 1]);
    }

    // ���������� ���������� �������� �������� �������� �������
    // ���������� std::abs ��� double
    return std::abs(totalSignedArea);
}


int main() {
    // ��������� ��������� �������� ����� � �������
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    int n, type;
    cout << "������� ���������� ������ ��������������: ";
    cin >> n;

    if (n < 3) {
        cout << "������: ������������� ������ ����� ������� 3 �������." << endl;
        return 1;
    }

    cout << "������� ��� ����� (1 - ���������, 2 - ������): ";
    cin >> type;
    vector<Point> vertices(n);
    if (type == 2) {
        cout << "������� ���������� ������ (x y) � ������� ������ �� ���������:" << endl;
        for (int i = 0; i < n; i++) {
            cout << "������� " << i + 1 << ": ";
            cin >> vertices[i].x >> vertices[i].y;
    }
    } else if (type == 1) {
        vertices = generateRandomPolygon(n);
    } else {
        cout << "������: �������� ��� �����." << endl;
        return 1;
    }
    

    // --- ��������� ������� ��� ������� ������ ---
    auto start_shoelace = chrono::high_resolution_clock::now();
    double area_shoelace = calculatePolygonArea_Shoelace(vertices);
    auto end_shoelace = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration_shoelace = end_shoelace - start_shoelace;

    cout << "\n--- ������� ������ (������) ---" << endl;
    cout << "������� ��������������: " << area_shoelace << endl;
    cout << "����� ����������: " << duration_shoelace.count() << " ��" << endl;

    // --- ��������� ������� ��� ������������ ---
    auto start_triangulation = chrono::high_resolution_clock::now();
    double area_triangulation = calculatePolygonArea_Triangulation(vertices);
    auto end_triangulation = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration_triangulation = end_triangulation - start_triangulation;

    cout << "\n--- ����� ������������ ---" << endl;
    cout << "������� ��������������: " << area_triangulation << endl;
    cout << "����� ����������: " << duration_triangulation.count() << " ��" << endl;
    // ������ ���������� � ����������, ��� ��� ����� ���������


    // �������� ����� ���������
    cout << "\n������� Enter ��� ����������...";
    // ���������� ������� ������ ����� ����� get()
    cin.ignore(); 
    cin.get();

    return 0;
}