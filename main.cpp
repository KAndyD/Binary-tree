#include "binary_tree.h"
#include <chrono>
#include <fstream>
#include <random>
#include <complex>
#include <cassert>

using namespace std;
using namespace std::chrono;

void test_all_features() {
    using T = int;
    BinaryTree<T> tree;
    tree.Insert(10);
    tree.Insert(5);
    tree.Insert(15);
    tree.Insert(3);
    tree.Insert(7);

    cout << "\n== Traversal Tests ==" << endl;
    cout << "In-order: ";
    tree.Traverse(TraversalType::IN_ORDER, [](T val) { cout << val << " "; });
    cout << "\nPre-order: ";
    tree.Traverse(TraversalType::PRE_ORDER, [](T val) { cout << val << " "; });
    cout << "\nPost-order: ";
    tree.Traverse(TraversalType::POST_ORDER, [](T val) { cout << val << " "; });
    cout << "\n";

    cout << "\n== Map Test ==" << endl;
    auto mapped = tree.map([](T val) { return val * 2; });
    mapped.Traverse(TraversalType::IN_ORDER, [](T val) { cout << val << " "; });
    cout << "\n";

    cout << "\n== Where Test ==" << endl;
    auto filtered = tree.where([](T val) { return val > 5; });
    filtered.Traverse(TraversalType::IN_ORDER, [](T val) { cout << val << " "; });
    cout << "\n";

    cout << "\n== Merge Test ==" << endl;
    BinaryTree<T> another;
    another.Insert(8);
    another.Insert(12);
    auto merged = tree.merge(another);
    merged.Traverse(TraversalType::IN_ORDER, [](T val) { cout << val << " "; });
    cout << "\n";

    cout << "\n== Extract Subtree Test ==" << endl;
    auto subtree = tree.extractSubtree(5);
    subtree.Traverse(TraversalType::IN_ORDER, [](T val) { cout << val << " "; });
    cout << "\n";

    cout << "\n== Contains Subtree Test ==" << endl;
    cout << "Tree contains subtree: " << (tree.containsSubtree(subtree) ? "Yes" : "No") << "\n";

    cout << "\n== Serialization/Deserialization Test ==" << endl;
    string serialized = tree.serialize(TraversalType::PRE_ORDER);
    cout << "Serialized tree (PRE_ORDER): " << serialized << "\n";
    BinaryTree<T> deserialized;
    deserialized.deserialize(serialized, TraversalType::PRE_ORDER);
    deserialized.Traverse(TraversalType::IN_ORDER, [](T val) { cout << val << " "; });
    cout << "\n";

    cout << "\n== Path Search Test ==" << endl;

    vector<string> valid_path = {"left"};  // путь от 10 к 5
    try {
        T result = tree.GetByPath(valid_path);
        cout << "GetByPath left: " << result << "\n";
    } catch (const exception& e) {
        cout << "GetByPath failed: " << e.what() << "\n";
    }

    vector<string> rel_path = {"right"};  // путь от 5 к 7
    try {
        T result = tree.GetByRelativePath(5, rel_path);
        cout << "GetByRelativePath from 5, right: " << result << "\n";
    } catch (const exception& e) {
        cout << "GetByRelativePath failed: " << e.what() << "\n";
    }
}


// Тестирование скорости операций
void performance_test() {
    ofstream out("performance.csv");
    out << "n,insert_time,find_time,remove_time\n";

    // Более скромные размеры для тестирования
    for (int n = 1000; n <= 10000; n *= 10) {
        BinaryTree<int> tree;
        vector<int> elements(n);
        
        // Генерируем уникальные значения для более реалистичного теста
        for (int i = 0; i < n; ++i) {
            elements[i] = i + 1;
        }
        
        // Перемешиваем значения
        random_device rd;
        mt19937 gen(rd());
        shuffle(elements.begin(), elements.end(), gen);

        // Тест вставки
        auto start = high_resolution_clock::now();
        for (int i = 0; i < n; ++i) {
            tree.Insert(elements[i]);
        }
        auto insert_time = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();

        // Тест поиска - используем первые 100 элементов для поиска
        start = high_resolution_clock::now();
        for (int i = 0; i < 100; ++i) {
            tree.Contains(elements[i]);
        }
        auto find_time = duration_cast<microseconds>(high_resolution_clock::now() - start).count();

        // Тест удаления - удаляем первые 100 элементов
        start = high_resolution_clock::now();
        for (int i = 0; i < 100; ++i) {
            tree.Remove(elements[i]);
        }
        auto remove_time = duration_cast<microseconds>(high_resolution_clock::now() - start).count();

        out << n << "," << insert_time << "," << find_time << "," << remove_time << "\n";
        cout << "Completed test for n = " << n << endl;
    }
}

void performance_test_large() {
    const int n = 1000000; // 10^6 элементов
    cout << "Running performance test with " << n << " elements..." << endl;
    
    BinaryTree<int> tree;
    vector<int> elements(n);
    
    // Заполняем последовательными числами и перемешиваем
    iota(elements.begin(), elements.end(), 1);
    random_device rd;
    mt19937 gen(rd());
    shuffle(elements.begin(), elements.end(), gen);
    
    // Тест вставки с прогресс-баром
    cout << "Inserting elements...";
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < n; ++i) {
        tree.Insert(elements[i]);
        if (i % 100000 == 0) cout << "." << flush; // Прогресс каждые 100к элементов
    }
    auto insert_time = chrono::duration_cast<chrono::milliseconds>(
        chrono::high_resolution_clock::now() - start).count();
    cout << "\nInsert time: " << insert_time << " ms" << endl;
    
    // Тест поиска (первые 1000 элементов)
    cout << "Searching elements...";
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        if (!tree.Contains(elements[i])) {
            cerr << "\nError: element not found!" << endl;
            return;
        }
    }
    auto search_time = chrono::duration_cast<chrono::milliseconds>(
        chrono::high_resolution_clock::now() - start).count();
    cout << "\nSearch time for 1000 elements: " << search_time << " ms" << endl;
    
    // Тест удаления (первые 1000 элементов)
    cout << "Removing elements...";
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        tree.Remove(elements[i]);
    }
    auto remove_time = chrono::duration_cast<chrono::milliseconds>(
        chrono::high_resolution_clock::now() - start).count();
    cout << "\nRemove time for 1000 elements: " << remove_time << " ms" << endl;
    
    // Проверка размера
    cout << "Final size (should be " << n-1000 << "): ";
    // Здесь нужно добавить метод Size() в ваш класс BinaryTree
    // cout << tree.Size() << endl;
}
// Модульные тесты
void unit_tests() {
    // Тест для int
    BinaryTree<int> int_tree;
    int_tree.Insert(5);
    int_tree.Insert(3);
    int_tree.Insert(7);
    
    try {
        int_tree.Contains(3);
        cout << "Int test passed\n";
    } catch (...) {
        cout << "Int test failed\n";
    }

    // Тест для string
    BinaryTree<string> str_tree;
    str_tree.Insert("apple");
    str_tree.Insert("banana");
    
    try {
        str_tree.Contains("apple");
        cout << "String test passed\n";
    } catch (...) {
        cout << "String test failed\n";
    }

    // Тест для complex
    BinaryTree<complex<double>> comp_tree;
    comp_tree.Insert({1.0, 2.0});
    comp_tree.Insert({3.0, 4.0});

    try {
        comp_tree.Contains({1.0, 2.0});
        cout << "Complex test passed\n";
    } catch (...) {
        cout << "Complex test failed\n";
    } 
}



int main() {
    cout << "Running unit tests...\n";
    unit_tests();

    cout << "Running performance tests...\n";
    performance_test();
    cout << "Results saved to performance.csv\n";

    cout << "Running performance tests large...\n";
    performance_test_large();
    cout << "Results saved to performance_large.csv\n";


    cout << "Running full feature test...\n";
    test_all_features();

    return 0;
}
