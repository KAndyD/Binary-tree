#include <iostream>

#ifndef BINARI_TREE_NODE_H
#define BINARY_TREE_NODE_H

template <typename T>
struct Node {
    T data; // значение, хранящееся в узле
    Node<T>* left; // указатель на левого потомка
    Node<T>* right; // указатель на правого потомка

    // Constructor
    // explicit запрещает неявное преобразование T к Node
    // Принимает const T& - константную ссылку на значение
    explicit Node(const T& value) : data(value), left(nullptr), right(nullptr) {}

    // Destructor (по умолчанию)
    // Для избегания double free
    ~Node() = default;

    // Создает глубокую копию поддерева (не изменяет текущий узел)
    // Все узлы копируются рекурсией
    Node<T>* copy() const {
        Node<T>* newNode = new Node<T>(data);
        
        if (left) newNode->left = left->copy();
        if (right) newNode->right = right->copy();
        
        // Возвращает указатель на корень скопированного поддерева
        return newNode;
    }

    // Проверяет, является ли узел листовым (нет потомков)
    bool isLeaf() const {
        // !left && !right - оба указателя должны быть nullptr
        return !left && !right;
    }
};

#endif