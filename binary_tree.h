#include <iostream>

#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include "node.h"
#include "exceptions.h"
#include <complex>
#include <functional> // Предоставляет функциональные объекты и поддержку лямбда-выражений
#include <vector> // Необходим для работы с путями в дереве (последовательность узлов)
#include <string> // Необходим для сериализации/десериализации дерева
#include <queue> // Предоставляет контейнеры std::queue (очередь) и std::priority_queue. Используется при десериализации дерева
#include <stack>  // Для использования std::stack
#include <memory> // Для std::unique_ptr (если будете использовать)

// Перечисление, которое определяет различные способы обхода (траверсировки) бинарного дерева
// enum class предотвращает неявное преобразование к int
// Позволяет единообразно работать с разными типами обходов
// Можно передавать тип обхода как параметр в функции -> Одна функция может использовать любой тип обхода
enum class TraversalType {
    PRE_ORDER,          // Корень → Лево → Право (КЛП)
    //Полезно для копирования дерева

    REVERSE_PRE_ORDER,  // Корень → Право → Лево (КПЛ)
    // Полезно для зеркального отображения дерева

    IN_ORDER,           // Лево → Корень → Право (ЛКП)
    // Для бинарных деревьев поиска даёт элементы в отсортированном порядке

    REVERSE_IN_ORDER,   // Право → Корень → Лево (ПКЛ)
    // Даёт элементы в обратном отсортированном порядке

    POST_ORDER,         // Лево → Право → Корень (ЛПК)
    // Полезно для удаления дерева

    REVERSE_POST_ORDER  // Право → Лево → Корень (ПЛК)  
    // Альтернативный вариант POST_ORDER
};


template <typename T>
class BinaryTree {
private:
    // Корень
    Node<T>* root;

    // Вспомогательные методы

    // Метод полной очистки дерева
    void Clear(Node<T>* node);
    // Внутренний (приватный) метод глубокого копирования поддерева
    Node<T>* Copy(Node<T>* node) const;
    // Рекурсивный поиск узла с указанным значением в поддереве
    Node<T>* FindNode(Node<T>* node, const T& value) const;
    // Рекурсивное удаление узла с указанным значением
    Node<T>* RemoveNode(Node<T>* node, const T& value);
    // Поиск узла с минимальным значением в поддереве
    Node<T>* FindMin(Node<T>* node) const;
    // Функция сравнения дереьвев (сугубо вспомогательная)
    bool CompareSubtrees(Node<T>* ourNode, Node<T>* subNode) const;


    // Методы обходов

    // Приватный метод обхода поддерева
    void Traverse(Node<T>* node, TraversalType type, std::function<void(T)> action) const;
    // Прямой обход (Корень → Лево → Право) 
    void PreOrder(Node<T>* node, std::function<void(T)> action) const;
    // Обратный прямой обход (Корень → Право → Лево)
    void ReversePreOrder(Node<T>* node, std::function<void(T)> action) const;
    // Симметричный обход (Лево → Корень → Право)
    void InOrder(Node<T>* node, std::function<void(T)> action) const;
    // Обратный симметричный обход (Право → Корень → Лево)
    void ReverseInOrder(Node<T>* node, std::function<void(T)> action) const;
    // Обратный обход (Лево → Право → Корень)
    void PostOrder(Node<T>* node, std::function<void(T)> action) const;
    // Обратный обратный обход (Право → Лево → Корень)
    void ReversePostOrder(Node<T>* node, std::function<void(T)> action) const;


    // Методы для сериализации
    /* Сериализация - это процесс преобразования структуры данных 
    (в нашем случае бинарного дерева) в последовательность байт (строку), 
    которую можно сохранить в файл или передать по сети. */

    // Сериализация в прямом порядке (Преобразует дерево в строку в порядке "Корень → Левое поддерево → Правое поддерево")
    void SerializePreOrder(Node<T>* node, std::string& result) const;
    // Сериализация в обратном прямом порядке (Преобразует дерево в строку в порядке "Корень → Правое поддерево → Левое поддерево")
    void SerializeReversePreOrder(Node<T>* node, std::string& result) const;
    // Сериализация в симметричном порядке (Преобразует дерево в строку в порядке "Левое поддерево → Корень → Правое поддерево")
    void SerializeInOrder(Node<T>* node, std::string& result) const;
    //  Сериализация в обратном симметричном порядке (Преобразует дерево в строку в порядке "Правое поддерево → Корень → Левое поддерево")
    void SerializeReverseInOrder(Node<T>* node, std::string& result) const;
    // Сериализация в обратном порядке (Преобразует дерево в строку в порядке "Левое поддерево → Правое поддерево → Корень")
    void SerializePostOrder(Node<T>* node, std::string& result) const;
    // Сериализация в обратном обратном порядке (Преобразует дерево в строку в порядке "Правое поддерево → Левое поддерево → Корень")
    void SerializeReversePostOrder(Node<T>* node, std::string& result) const;

    // Методы для десериализации
    /*Десериализация - обратный процесс восстановления 
    структуры данных из последовательности байт.*/

    // Десериализация дерева из PreOrder представления
    Node<T>* DeserializePreOrder(std::queue<std::string>& elements);
    Node<T>* DeserializeReversePreOrder(std::queue<std::string>& elements);
    Node<T>* DeserializeInOrder(std::queue<std::string>& elements);
    Node<T>* DeserializeReverseInOrder(std::queue<std::string>& elements);
    // Десериализация дерева из PostOrder представления
    Node<T>* DeserializePostOrder(std::queue<std::string>& elements);
    Node<T>* DeserializeReversePostOrder(std::queue<std::string>& elements);


public:

    // Количество узлов в бинарном дереве
    size_t Size() const {
        size_t count = 0; // Счетчик
        // Вызов обхода дерева
        // Для каждого узла вызывается лямбда функция и увеличивает счетчик
        Traverse(TraversalType::IN_ORDER, [&count](const T&){ ++count; });
        return count;
    }


    // Конструкторы и деструктор

    // Kонструтор по умолчанию
    BinaryTree();
    // Конструктор с параметром 
    explicit BinaryTree(const T& rootValue);
    // Конструктор копирования
    BinaryTree(const BinaryTree& other);
    // Конструктор перемещения 
    BinaryTree(BinaryTree&& other) noexcept;
    // Деструктор
    ~BinaryTree();


    // Операторы присваивания

    // Оператор присваивания копированием
    BinaryTree& operator=(const BinaryTree& other);
    // Оператор присваивания перемещением
    BinaryTree& operator=(BinaryTree&& other) noexcept;


    // Основные операции

    // Вставка значения - добавление нового узла с указанным значением в дерево
    void Insert(const T& value);
    // Метод проверки существования значения
    bool Contains(const T& value) const;
    // Удаление значения (с сохранением структуры дерева)
    void Remove(const T& value);
    // Проверка пустоты
    bool IsEmpty() const;
    void Clear();

    // Обход дерева
    void Traverse(TraversalType type, std::function<void(T)> action) const;


    // Функциональные операции

    // Трансформация значений (применение функции-маппера к каждому элементу исходного дерева)
    BinaryTree<T> map(std::function<T(T)> mapper) const;
    // Фильтрация элементов (Создание нового дерева, включающего только те элементы, которые удовлетворяют условию)
    BinaryTree<T> where(std::function<bool(T)> predicate) const;
    // Cлияние деревьев (создание нового)
    BinaryTree<T> merge(const BinaryTree<T>& other) const;


    // Работа с поддеревьями

    // Извлечение поддерева (Создание новое дерева, которое является копией поддерева, начиная с узла с указанным значением)
    BinaryTree<T> extractSubtree(const T& value) const;
    // Проверка наличия поддерева
    bool containsSubtree(const BinaryTree<T>& subtree) const;


    // Сериализация/десериализация
    std::string serialize(TraversalType type = TraversalType::PRE_ORDER) const;
    void deserialize(const std::string& data, TraversalType type = TraversalType::PRE_ORDER);


    // Поиск по пути

    // Получение значения по абсолютному пути от корня
    T GetByPath(const std::vector<std::string>& path) const;
    // Получение значения по относительному пути от узла с указанным значением
    T GetByRelativePath(const T& base, const std::vector<std::string>& path) const;
};
#endif