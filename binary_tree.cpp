#include <iostream>
#include "binary_tree.h" // Заголовочный файл класса
#include <algorithm>
#include <sstream> // Для работы со строками как с потоками(в сериализации)
#include "exceptions.h" // Исключения
#include <complex>
#include <stack>
#include <obstack.h>

// Специализация для complex<double>
namespace std {
    bool operator<(const complex<double>& a, const complex<double>& b) {
        if (a.real() != b.real()) {
            return a.real() < b.real();
        }
        return a.imag() < b.imag();
    }
    
    bool operator>(const complex<double>& a, const complex<double>& b) {
        if (a.real() != b.real()) {
            return a.real() > b.real();
        }
        return a.imag() > b.imag();
    }
}

namespace std {
    string to_string(const complex<double>& c) {
        return "(" + to_string(c.real()) + "," + to_string(c.imag()) + ")";
    }
}

// Kонструтор по умолчанию
template <typename T>
BinaryTree<T>::BinaryTree() : root(nullptr) {} // Корень дерева в nullptr


// Конструктор с параметром 
template <typename T>
BinaryTree<T>::BinaryTree(const T& rootValue){ // Принимает константную ссылку на значение корня
    try { // Блок обработки исключений 
        root = new Node<T>(rootValue); // Выделение памяти для нового узла
    }
    // Перехват исключения при нехватки памяти
    catch (const std::bad_alloc&) {
        throw TreeException("Memory allocation failed in constructor"); // Преобразование системного исключения в exceptions
    }
}

// Деструктор
template <typename T>
BinaryTree<T>::~BinaryTree(){
    // Очистка деревa
        Clear();
}

// Конструктор копирования
template <typename T>
BinaryTree<T>::BinaryTree(const BinaryTree& other) { // other - исходное дерево для копирования
    try {
        root = other.root ? Copy(other.root) : nullptr; /*тернарный оператор:
                                                        Если other.root существует, вызывает Copy()
                                                        Иначе устанавливает nullptr*/
    }
    // Перехват нехватки памяти
    catch (const std::bad_alloc&){
        throw TreeException("Memory allocation failed in copy constructor");
    }
    // Перехват любых иных исключений
    catch (...) {
        throw TreeException("Unknown error during tree copying");
    }
}

// Конструктор перемещения 
// noexcept - гарантия отсутствия ошибок
template <typename T>
BinaryTree<T>::BinaryTree(BinaryTree&& other) noexcept : root(other.root) { // Инициализация корня значением корня другого обьекта
    other.root = nullptr; // Обнуление указателя в исходном обьекте
}

// Внутренний (приватный) метод рекурсивной очистки поддерева
// Полное удаление всех узлов, начиная с заданного узла
/*template <typename T>
void BinaryTree<T>::Clear(Node<T>* node) { // указатель на корневой узел поддерева для удаления
    if (!node) {
        return; // Проверка на nullptr - выход из рекурсии
    }

    Node<T>* newNode = nullptr; 
    // Создание нового узла с обработкой bad_alloc
    try {
        newNode = new Node<T>(node->data); // Создание копии узла
    }
    catch (const std::bad_alloc&) {
        throw TreeException("Memory allocation failed for node copy");
    }
    
    // Рекурсивное копирование левого поддерева
    try {
        newNode->left = Copy(node->left);
    }
    catch (...) {
        delete newNode; // Очистка частично скопированных данных
        throw TreeException("Failed to copy left subtree");
    }

    // Рекурсивное копирование правого поддерева
    try {
        newNode->right = Copy(node->right);  
    }
    catch (...) {
        delete newNode->left;  // Очистка
        delete newNode; // Очистка частично скопированных данных
        throw TreeException("Failed to copy right subtree");
    }

    
}*/

template <typename T>
void BinaryTree<T>::Clear() {
    if (!root) return;
    
    std::stack<Node<T>*> nodeStack;
    nodeStack.push(root);
    
    while (!nodeStack.empty()) {
        Node<T>* current = nodeStack.top();
        nodeStack.pop();
        
        if (current->left) nodeStack.push(current->left);
        if (current->right) nodeStack.push(current->right);
        
        delete current;
    }
    
    root = nullptr;
}

template <typename T>
void BinaryTree<T>::Clear(Node<T>* node) {
    if (!node) {
        return;
    }
    
    // Рекурсивное удаление потомков
    Clear(node->left);
    Clear(node->right);
    
    node->left = nullptr;  // Явное обнуление указателей перед удалением
    node->right = nullptr;
    // Удаление текущего узла
    delete node;
}

// Метод полной очистки дерева
// Удаление всех узлов дерева и сброс корня
// Гарантирует, что root станет nullptr даже при ошибках
/*template <typename T>
void BinaryTree<T>::Clear() {
    // Проверка на пустое дерево
    if (!root) {
        return; 
    }

    try {
        Clear(root); // Вызов рекурсивной очистки
    }
    catch (const TreeException& e) {
        root = nullptr; // Гарантия согласованности состояния 
        throw; // Переброс исключения далее
    }
    catch (...) {
        root = nullptr;
        throw TreeException("Unknown error during tree clearance");
    }

    root = nullptr; // Успешная очистка
}*/


// Внутренний (приватный) метод глубокого копирования поддерева
template <typename T>
Node<T>* BinaryTree<T>::Copy(Node<T>* node) const {
    // Базовый случай рекурсии
    if (!node) {
        return nullptr; 
    }

    Node<T>* newNode = nullptr; // Сброс newNode на nullptr(гарантированно)

    try {
        newNode = new Node<T>(node->data); // Создание копии узла
    }
    catch (const std::bad_alloc&) { // Если памяти нет
        throw TreeException("Memory allocation failed for node copy");
    }

    // Рекурсивное копирование левого поддерева
    try {
        newNode->left = Copy(node->left);
    }
    catch (...) {
        delete newNode; // Очистка частично скопированных данных
        throw TreeException("Failed to copy left subtree");
    }

    // Рекурсивное копирование правого поддерева
    try {
        newNode->right = Copy(node->right);  // Рекурсивное копирование правого поддерева
    }
    catch (...) {
        delete newNode->left;  // Очистка
        delete newNode;
        throw TreeException("Failed to copy right subtree");
    }

    return newNode;  // Возврат готовой копии

}


// Оператор присваивания копированием
// Очищает текущее дерево и создает копию другого
template <typename T>
BinaryTree<T>& BinaryTree<T>::operator=(const BinaryTree& other) { // other - исходное дерево для копирования
    // Проверка на самоприсваивание
    if (this != &other) {
        try {
            Clear(); // Очистка текущего дерева
            root = other.root ? Copy(other.root) : nullptr; // Копирование
        }
        catch (const std::bad_alloc&) {
            root = nullptr; // Гарантия согласованного состояния
            throw TreeException("Memory allocation failed in assignment");
        }
        catch (...) {
            root = nullptr;
            throw TreeException("Unknown error during tree assignment");
        }
    }

    return *this; // возврат текущего обьекта
}

// Оператор присваивания перемещением
// Освобождает текущие ресурсы и захватывает чужие
template <typename T>
BinaryTree<T>& BinaryTree<T>::operator=(BinaryTree&& other) noexcept { // other - временный обьект
    // Провекрка на самоприсваивание
    if (this != &other) {
        Clear(); // Очистка текущих данных
        root = other.root; // Захват указателя
        other.root = nullptr; // Обнуление исходного указателя
    }

    return *this; // Возврат текущего объекта
}



// Рекурсивный поиск узла с указанным значением в поддереве
template <typename T>
Node<T>* BinaryTree<T>::FindNode(Node<T>* node, const T& value) const {
     // Базовый случай: узел не найден
    if (!node) {
        return nullptr;
    }
    // Значение найдено
    if (value == node->data) {
        return node;
    }
    // сравнение для определения направления поиска
    if (value < node->data) {
        return FindNode(node->left, value); // Поиск в левом поддеревe
    }
    else {
        return FindNode(node->right, value); // Поиск в правом поддеревe
    }
}

// Поиск узла с минимальным значением в поддереве
template <typename T>
Node<T>* BinaryTree<T>::FindMin(Node<T>* node) const {
    // Пустое поддерево
    if (!node) {
        return nullptr;
    }
    // Итеративный спуск по левым узлам
    while (node->left) {
        node = node->left;
    }
    return node; // Возврат узла без левых потомков
}

// Рекурсивное удаление узла с указанным значением
template <typename T>
Node<T>* BinaryTree<T>::RemoveNode(Node<T>* node, const T& value) {
    // Узел не найден
    if (!node) {
        return nullptr;
    }
    // Рекурсивный поиск узла
    try {
        // сравнение для определения направления поиска
        if (value < node->data) {
            node->left = RemoveNode(node->left, value); // Поиск в левом поддереве
        }
        else if (value > node->data) {
            node->right = RemoveNode(node->right, value); // Поиск в правом поддереве
        }
        else { // Найден узел для удаления
            if (!node->left) { // Нет левого поддерева
                Node<T>* temp = node->right;
                node->right = nullptr; // Обнуление перед удалением
                delete node;
                return temp;
            }
            else if (!node->right) {    // Нет правого поддерева
                Node<T>* temp = node->left;
                node->left = nullptr;   // Обнуление перед удалением
                delete node;
                return temp;
            }
            // Есть оба поддерева
            Node<T>* temp = FindMin(node->right); // Поиск минимума справа
            node->data = temp->data; // Копирование данных
            node->right = RemoveNode(node->right, value); // Удаление дубликата
        }
    }
    catch (...) {
        throw TreeException("Failed to remove node");
    }

    return node; // Возврат текущего узла
}

// Вставка значения - добавление нового узла с указанным значением в дерево
template <typename T>
void BinaryTree<T>::Insert(const T& value) {
    // Случай пустого дерева
    if (!root) {
        try {
            root = new Node<T>(value); // Попытка создания корня
        }
        catch (const std::bad_alloc&) {
            throw TreeException("Memory allocation failed for root node");
        }
        return;
    }

    Node<T>* current = root;
    while (1) {
        if (value < current->data) { // В левое поддерево
            if (!current->left) {
                try {
                    current->left = new Node<T>(value); // Вставка слева
                    break;
                }
                catch (const std::bad_alloc&) {
                    throw TreeException("Memory allocation failed for left node");
                }
            }
            current = current->left;
        }
        else if (value > current->data) { // В правое поддерево
            if (!current->right) {
                try {
                    current->right = new Node<T>(value); // Вставка справа
                    break;
                }
                catch (const std::bad_alloc&) {
                    throw TreeException("Memory allocation failed for right node");
                }
            }
            current = current->right;
        }
        
    }
}


// Метод проверки существования значения
template <typename T>
bool BinaryTree<T>::Contains(const T& value) const {
    // Проверка на пустоту
    if (IsEmpty()) {
        throw TreeException("Tree is empty - cannot check containment");
    }

    // Старт с корня
    Node<T>* current = root;
    
    while (current != nullptr) { // Пока есть узлы для проверки
        if (value == current->data) { // Значение найдено
            return true;
        }
        else if (value < current->data) { // Движение влево
            if (!current->left) {
                throw TreeException("Value not found - left subtree ended");
            }
            current = current->left;
        }
        else { // Движение вправо
            if (!current->right) { 
                throw TreeException("Value not found - right subtree ended");
            }
            current = current->right;
        }
    }
    
    return false; // Не найдено
}



// Удаление значения (с сохранением структуры дерева)
template <typename T>
void BinaryTree<T>::Remove(const T& value) {
    // Проверка существования
    if (!Contains(value)) {
        throw TreeException("Cannot remove - value not found in tree");
    }

    try {
        root =  RemoveNode(root, value); // Вызов внутренней функции (основной алгоритм удаления)
    }
    catch (...) {
        throw TreeException("Failed to remove node");
    }
}

// Проверка пустоты
template <typename T>
bool BinaryTree<T>::IsEmpty() const{
    return root == nullptr; // Если нет корня - значит дерево пустое
}



// Приватный метод обхода поддерева
// Параметры: корень поддерева для обхода, тип обхода, функция обработки элементов
template <typename T>
void BinaryTree<T>::Traverse(Node<T>* node, TraversalType type, std::function<void(T)> action) const {
    // Пустое поддерево - выход
    if (!node) {
        return;
    }
    // Проверка функции обработки
    if (!action) {
        throw TreeException("Action function cannot be null");
    }

    try {
        switch (type) { // Ветвление по типу обхода
            case TraversalType::PRE_ORDER: // Корень → Лево → Право (КЛП)
                PreOrder(node, action);
                break;
            case TraversalType::REVERSE_PRE_ORDER: // Корень → Право → Лево (КПЛ)
                ReversePreOrder(node, action);
                break;
            case TraversalType::IN_ORDER: // Лево → Корень → Право (ЛКП)
                InOrder(node, action);
                break;
            case TraversalType::REVERSE_IN_ORDER: // Право → Корень → Лево (ПКЛ)
                ReverseInOrder(node, action);
                break;
            case TraversalType::POST_ORDER: // Лево → Право → Корень (ЛПК)
                PostOrder(node, action);
                break;
            case TraversalType::REVERSE_POST_ORDER: // Право → Лево → Корень (ПЛК)
                ReversePostOrder(node, action);
                break;
            default:
                throw TreeException("Invalid traversal type for subtree");
        }
    }
    catch (const std::exception& e){
        throw TreeException(std::string("Subtree traversal failed: ") + e.what());
    }
}

// Публичный метод обхода дерева
// Параметры: тип обхода, функция, применяемая к каждому узлу
template <typename T>
void BinaryTree<T>::Traverse(TraversalType type, std::function<void(T)> action) const {
    Node<T>* node = root;
    // Bалидация переданной функции
    if (!action) {
        throw TreeException("Action function cannot be null");
    }

    try {
        switch (type) { // Ветвление по типу обхода
            case TraversalType::PRE_ORDER: // Корень → Лево → Право (КЛП)
                PreOrder(node, action);
                break;
            case TraversalType::REVERSE_PRE_ORDER: // Корень → Право → Лево (КПЛ)
                ReversePreOrder(node, action);
                break;
            case TraversalType::IN_ORDER: // Лево → Корень → Право (ЛКП)
                InOrder(node, action);
                break;
            case TraversalType::REVERSE_IN_ORDER: // Право → Корень → Лево (ПКЛ)
                ReverseInOrder(node, action);
                break;
            case TraversalType::POST_ORDER: // Лево → Право → Корень (ЛПК)
                PostOrder(node, action);
                break;
            case TraversalType::REVERSE_POST_ORDER: // Право → Лево → Корень (ПЛК)
                ReversePostOrder(node, action);
                break;
            default: // Обработка недопустимых значений перечисления
                throw TreeException("Invalid traversal type specified");
        }
    }
    catch (const std::exception& e) {
        throw TreeException(std::string("Traversal failed: ") + e.what());
    }
}

// Прямой обход (Корень → Лево → Право) 
// Параметры: текущий узел, функция обработки
template <typename T>
void BinaryTree<T>::PreOrder(Node<T>* node, std::function<void(T)> action) const {
    // Базовый случай рекурсии (выход)
    if (!node) {
        return;
    }

    try {
        action(node->data); // Обработка текущего узла
    }
    catch (...) {
        throw TreeException("Action failed during PreOrder traversal");
    }

    PreOrder(node->left, action); // Левое поддерево
    PreOrder(node->right, action); // Правое поддерево
}

// Обратный прямой обход (Корень → Право → Лево)
// Параметры: текущий узел, функция обработки
template <typename T>
void BinaryTree<T>::ReversePreOrder(Node<T>* node, std::function<void(T)> action) const {
    // Базовый случай рекурсии (выход)
    if (!node) {
        return;
    }

    try {
        action(node->data); // Обработка текущего узла
    }
    catch (...) {
        throw TreeException("Action failed during ReversePreOrder traversal");
    }
    
    ReversePreOrder(node->right, action); // Правое поддерево
    ReversePreOrder(node->left, action);  // Левое поддерево
}

// Симметричный обход (Лево → Корень → Право)
// Для BST(Binary Search Tree) возвращает отсортированную последовательность
// Параметры: текущий узел, функция обработки
template <typename T>
void BinaryTree<T>::InOrder(Node<T>* node, std::function<void(T)> action) const {
    // Базовый случай рекурсии (выход)
    if (!node) {
        return;
    } 
    
    InOrder(node->left, action); // Левое поддерево
    
    try {
        action(node->data); // Текущий узел
    }
    catch (...) {
        throw TreeException("Action failed during InOrder traversal");
    }
    
    InOrder(node->right, action); // Правое поддерево
}

// Обратный симметричный обход (Право → Корень → Лево)
// Для BST возвращает элементы в обратном порядке
// Параметры: текущий узел, функция обработки
template <typename T>
void BinaryTree<T>::ReverseInOrder(Node<T>* node, std::function<void(T)> action) const {
    // Базовый случай рекурсии (выход)
    if (!node) {
        return;
    }
    ReverseInOrder(node->right, action); // Правое поддерево
    
    try {
        action(node->data); // Текущий узел
    }
    catch (...) {
        throw TreeException("Action failed during ReverseInOrder traversal");
    }
    
    ReverseInOrder(node->left, action); // Левое поддерево
}

// Обратный обход (Лево → Право → Корень)
// Параметры: текущий узел, функция обработки
template <typename T>
void BinaryTree<T>::PostOrder(Node<T>* node, std::function<void(T)> action) const {
    // Базовый случай рекурсии (выход)
    if (!node) {
        return;
    }
    PostOrder(node->left, action); // Левое поддерево
    PostOrder(node->right, action); // Правое поддерево
    
    try {
        action(node->data); // Текущий узел
    }
    catch (...) {
        throw TreeException("Action failed during PostOrder traversal");
    }
}

// Обратный обратный обход (Право → Лево → Корень)
// Параметры: текущий узел, функция обработки
template <typename T>
void BinaryTree<T>::ReversePostOrder(Node<T>* node, std::function<void(T)> action) const {
    // Базовый случай рекурсии (выход)
    if (!node) {
        return;
    }
    ReversePostOrder(node->right, action); // Правое поддерево
    ReversePostOrder(node->left, action); // Левое поддерево
    
    try {
        action(node->data); // Текущий узел
    }
    catch (...) {
        throw TreeException("Action failed during ReversePostOrder traversal");
    }
}


// Трансформация значений (применение функции-маппера к каждому элементу исходного дерева)
template <typename T>
BinaryTree<T> BinaryTree<T>::map(std::function<T(T)> mapper) const {
    // Валидация переданной функции
    if (!mapper) {
        throw TreeException("Mapper function cannot be null");
    }

    BinaryTree<T> result; // Создание пустого дерева result для результатов
    if (!root) {
        return result; // Возврат пустого дерева
    }

    try {
        // Oбход PreOrder для сохранения структуры
        Traverse(TraversalType::PRE_ORDER, [&](const T& value) {
            try {
                T newValue = mapper(value); // Применение маппера
                result.Insert(newValue); // Добавление в новое дерево
            }
            catch (...) {
                throw TreeException("Mapper function execution failed");
            }
        });
    }
    catch (const TreeException&) {
        throw; // Далее
    }
    catch (...) {
        throw TreeException("Unknown error during map operation");
    }

    return result;
}

// Фильтрация элементов (Создание нового дерева, включающего только те элементы, которые удовлетворяют условию)
template <typename T>
BinaryTree<T> BinaryTree<T>::where(std::function<bool(T)> predicate) const {
    // Валидация переданной функции
    if (!predicate) {
        throw TreeException("Predicate function cannot be null");
    }

    BinaryTree<T> result; // Создание пустого дерева result для результатов
    if (!root) {
        return result; // Возврат пустого дерева
    }
    try {
        Traverse(TraversalType::IN_ORDER, [&](const T& value) {
            try {
                if (predicate(value)) { // Проверка условия
                    result.Insert(value); // Вставка при соответствии
                }
            }
            catch (...) {
                throw TreeException("Predicate function execution failed");
            }
        });
    }
    catch (const TreeException&) {
        throw;
    }
    catch (...) {
        throw TreeException("Unknown error during where operation");
    }

    return result;
}

// Cлияние деревьев (создание нового)
template <typename T>
BinaryTree<T> BinaryTree<T>::merge(const BinaryTree<T>& other) const {
    BinaryTree<T> result = *this; // Копирование текущего дерева

    try {
        // Добавление всех элементов из другого дерева (проходит по всем элементам второго дерева)
        other.Traverse(TraversalType::IN_ORDER, [&](const T& value) {
            try {
                result.Insert(value); // Попытка вставить каждый элемент
            }
            catch (const TreeException& e) {
                // Игнор дубликатов
                // Ошибки вставки логируются, но не прерывают процесс
                std::cerr << "Merge warning: " << e.what() << std::endl;
            }
            catch (...) {
                throw TreeException("Insertion failed during merge");
            }
        });
    }
    catch (const TreeException&) {
        throw;
    }
    catch (...) {
        throw TreeException("Unknown error during merge operation");
    }

    return result;  // Возврат обьединенного дерева
}



// Извлечение поддерева (Создание новое дерева, которое является копией поддерева, начиная с узла с указанным значением)
template <typename T>
BinaryTree<T> BinaryTree<T>::extractSubtree(const T& value) const {
    // Нахождение узела-кореня поддерева
    Node<T>* subtreeRoot = FindNode(root, value);
    
    // Проверка наличия узла
    if (!subtreeRoot) {
        throw NodeNotFound("Value not found in tree - cannot extract subtree");
    }

    BinaryTree<T> result;
    try {
        // Копирование поддерева начиная с найденного узла
        result.root = Copy(subtreeRoot);
    }
    catch (const std::bad_alloc&) {
        throw TreeException("Memory allocation failed during subtree extraction");
    }
    catch (...) {
        throw TreeException("Unknown error during subtree extraction");
    }

    // Возврат результата
    return result;
}

// Проверка наличия поддерева
template <typename T>
bool BinaryTree<T>::containsSubtree(const BinaryTree<T>& subtree) const {
    // Проверка на пустое поддерево
    if (subtree.IsEmpty()) {
        throw TreeException("Cannot search for empty subtree");
    }

    // Нахождение потенциального кореня поддерева
    Node<T>* potentialRoot = FindNode(root, subtree.root->data);
    
    // Если корень не найден - поддерева нет
    if (!potentialRoot) {
        return false;
    }

    // Сравнение структур деревьев
    try {
        return CompareSubtrees(potentialRoot, subtree.root);
    }
    catch (...) {
        throw TreeException("Error during subtree comparison");
    }
}

// Функция сравнения дереьвев (сугубо вспомогательная)
template <typename T>
bool BinaryTree<T>::CompareSubtrees(Node<T>* ourNode, Node<T>* subNode) const {
    // Базовый случай: если поддерево закончилось
    if (!subNode) {
        return true;
    }
    // Если наше дерево закончилось, а поддерево - нет
    if (!ourNode) {
        return false;
    }
    // Сравнение значений и рекурсивная проверка потомков
    return (ourNode->data == subNode->data) &&
           CompareSubtrees(ourNode->left, subNode->left) &&
           CompareSubtrees(ourNode->right, subNode->right);
}



// Получение значения по абсолютному пути от корня
// path - вектор направлений ("left"/"right")
template <typename T>
T BinaryTree<T>::GetByPath(const std::vector<std::string>& path) const {
    Node<T>* current = root;  // Старт с корня дерева
    
    // Проверка на пустое дерево
    if (!current) {
        throw NodeNotFound("Tree is empty - path cannot be traversed");
    }
    
    // Пошаговый обход пути
    for (const auto& direction : path) {
        // Проверка валидности направления
        if (direction != "left" && direction != "right") {
            throw InvalidTreeOperation("Invalid path direction: " + direction);
        }
        
        // Переход по указанному направлению
        if (direction == "left") {
            current = current->left;
        } else {
            current = current->right;
        }
        
        // Проверка существования узла
        if (!current) {
            throw NodeNotFound("Path leads to non-existent node");
        }
    }
    
    // Возврат значения найденного узла
    return current->data;
}

// Получение значения по относительному пути от узла с указанным значением
// base - значение базового узла
// вектор направлений ("left"/"right")
template <typename T>
T BinaryTree<T>::GetByRelativePath(const T& base, const std::vector<std::string>& path) const {
    // Нахождение базового узла
    Node<T>* baseNode = FindNode(root, base);
    // Не существует
    if (!baseNode) {
        throw NodeNotFound("Base node with value not found");
    }
    
    Node<T>* current = baseNode;  // Старт с базового узла
    
    // Пошаговый обход пути
    for (const auto& direction : path) {
        // Проверка валидности направления
        if (direction != "left" && direction != "right") {
            throw InvalidTreeOperation("Invalid path direction: " + direction);
        }
        
        // Переход по указанному направлению
        if (direction == "left") {
            current = current->left;
        } else {
            current = current->right;
        }
        
        // Проверка существования узла
        if (!current) {
            throw NodeNotFound("Path leads to non-existent node from base");
        }
    }
    
    // Возврат значения найденного узла
    return current->data;
}



// Сериализация дерева в строку
template <typename T>
std::string BinaryTree<T>::serialize(TraversalType type) const {
    std::string result;
    try {
        switch (type) {
            case TraversalType::PRE_ORDER:
                SerializePreOrder(root, result);
                break;
            case TraversalType::REVERSE_PRE_ORDER:
                SerializeReversePreOrder(root, result);
                break;
            case TraversalType::IN_ORDER:
                SerializeInOrder(root, result);
                break;
            case TraversalType::REVERSE_IN_ORDER:
                SerializeReverseInOrder(root, result);
                break;
            case TraversalType::POST_ORDER:
                SerializePostOrder(root, result);
                break;
            case TraversalType::REVERSE_POST_ORDER:
                SerializeReversePostOrder(root, result);
                break;
            default:
                throw TreeException("Unsupported serialization type");
        }
    }
    catch (...) {
        throw TreeException("Serialization failed");
    }
    return result;
}

// Сериализация в прямом порядке (Преобразует дерево в строку в порядке "Корень → Левое поддерево → Правое поддерево")
template <typename T>
void BinaryTree<T>::SerializePreOrder(Node<T>* node, std::string& result) const {
    if (!node) {
        result += "null ";  // Маркер отсутствия узла
        return;
    }
    
    try {
        if constexpr (std::is_same<T, std::string>::value) {
        // Если data — это строка, просто добавляем её
        result += node->data + " ";
    } else {
        // Если data — это не строка, преобразуем её в строку
        result += std::to_string(node->data) + " ";
    }  // Сериализация текущего узла
        SerializePreOrder(node->left, result);       // Левое поддерево
        SerializePreOrder(node->right, result);      // Правое поддерево
    }
    catch (...) {
        throw TreeException("PreOrder serialization failed");
    }
}

// Сериализация в обратном прямом порядке (Преобразует дерево в строку в порядке "Корень → Правое поддерево → Левое поддерево")
template <typename T>
void BinaryTree<T>::SerializeReversePreOrder(Node<T>* node, std::string& result) const {
    if (!node) {
        result += "null ";
        return;
    }
    
    try {
        if constexpr (std::is_same<T, std::string>::value) {
        // Если data — это строка, просто добавляем её
        result += node->data + " ";
    } else {
        // Если data — это не строка, преобразуем её в строку
        result += std::to_string(node->data) + " ";
    }  // Текущий узел
        SerializeReversePreOrder(node->right, result); // Правое поддерево
        SerializeReversePreOrder(node->left, result);  // Левое поддерево
    }
    catch (...) {
        throw TreeException("ReversePreOrder serialization failed");
    }
}

// Сериализация в симметричном порядке (Преобразует дерево в строку в порядке "Левое поддерево → Корень → Правое поддерево")
template <typename T>
void BinaryTree<T>::SerializeInOrder(Node<T>* node, std::string& result) const {
    if (!node) {
        result += "null ";
        return;
    }
    
    try {
        SerializeInOrder(node->left, result);       // Левое поддерево
        if constexpr (std::is_same<T, std::string>::value) {
        // Если data — это строка, просто добавляем её
        result += node->data + " ";
    } else {
        // Если data — это не строка, преобразуем её в строку
        result += std::to_string(node->data) + " ";
    } // Текущий узел
        SerializeInOrder(node->right, result);      // Правое поддерево
    }
    catch (...) {
        throw TreeException("InOrder serialization failed");
    }
}

//  Сериализация в обратном симметричном порядке (Преобразует дерево в строку в порядке "Правое поддерево → Корень → Левое поддерево")
template <typename T>
void BinaryTree<T>::SerializeReverseInOrder(Node<T>* node, std::string& result) const {
    if (!node) {
        result += "null ";
        return;
    }
    
    try {
        SerializeReverseInOrder(node->right, result); // Правое поддерево
        if constexpr (std::is_same<T, std::string>::value) {
        // Если data — это строка, просто добавляем её
        result += node->data + " ";
    } else {
        // Если data — это не строка, преобразуем её в строку
        result += std::to_string(node->data) + " ";
    }  // Текущий узел
        SerializeReverseInOrder(node->left, result);  // Левое поддерево
    }
    catch (...) {
        throw TreeException("ReverseInOrder serialization failed");
    }
}

// Сериализация в обратном порядке (Преобразует дерево в строку в порядке "Левое поддерево → Правое поддерево → Корень")
template <typename T>
void BinaryTree<T>::SerializePostOrder(Node<T>* node, std::string& result) const {
    if (!node) {
        result += "null ";
        return;
    }
    
    
        SerializePostOrder(node->left, result);      // Левое поддерево
        SerializePostOrder(node->right, result);     // Правое поддерево
        if constexpr (std::is_same<T, std::string>::value) {
        // Если data — это строка, просто добавляем её
        result += node->data + " ";
    } else {
        // Если data — это не строка, преобразуем её в строку
        result += std::to_string(node->data) + " ";
    }
    
}

// Сериализация в обратном обратном порядке (Преобразует дерево в строку в порядке "Правое поддерево → Левое поддерево → Корень")
template <typename T>
void BinaryTree<T>::SerializeReversePostOrder(Node<T>* node, std::string& result) const {
    if (node == nullptr) {
        return;  // Если узел пустой, ничего не делаем
    }

    // Сначала сериализуем правое поддерево
    SerializeReversePostOrder(node->right, result);
    
    // Потом сериализуем левое поддерево
    SerializeReversePostOrder(node->left, result);
    
    // Сериализуем текущий узел
    if constexpr (std::is_same<T, std::string>::value) {
        // Если data — это строка, просто добавляем её
        result += node->data + " ";
    } else {
        // Если data — это не строка, преобразуем её в строку
        result += std::to_string(node->data) + " ";
    }
}




// Десериализация дерева из строки
template <typename T>
void BinaryTree<T>::deserialize(const std::string& data, TraversalType type) {
    Clear(); // Очистка текущего дерева
    
    // Разбиение строки на токены
    std::queue<std::string> elements;
    std::istringstream iss(data);
    std::string token;
    
    while (iss >> token) { // Считывание токенов из строки
        elements.push(token);
    }

    try {
        // Выбор метода десериализации
        switch (type) {
            case TraversalType::PRE_ORDER:
                root = DeserializePreOrder(elements);
                break;
            case TraversalType::REVERSE_PRE_ORDER:
                root = DeserializeReversePreOrder(elements);
                break;
            case TraversalType::IN_ORDER:
                root = DeserializeInOrder(elements);
                break;
            case TraversalType::REVERSE_IN_ORDER:
                root = DeserializeReverseInOrder(elements);
                break;
            case TraversalType::POST_ORDER:
                root = DeserializePostOrder(elements);
                break;
            case TraversalType::REVERSE_POST_ORDER:
                root = DeserializeReversePostOrder(elements);
                break;
            default:
                throw TreeException("Unsupported deserialization type");
        }
        
        // Проверка лишних токенов
        if (!elements.empty()) {
            throw TreeException("Extra data in input string");
        }
    }
    catch (...) {
        Clear(); // В случае ошибки очистка дерева
        throw TreeException("Deserialization failed");
    }
}

// Десериализация дерева из PreOrder представления
// elements - очередь токенов ("значение" или "null")
template <typename T>
Node<T>* BinaryTree<T>::DeserializePreOrder(std::queue<std::string>& elements) {
    if (elements.empty()) {
        return nullptr; // Нет данных - возврат nullptr
    }
    std::string token = elements.front(); // Следующий токен
    elements.pop();
    
    if (token == "null") {
        return nullptr; // Токен "null" означает пустой узел
    }
    try {
        T value;
        std::istringstream(token) >> value; // Парсинг значения
        Node<T>* node = new Node<T>(value); // Создание узла
        
        // Рекурсивное строительство поддеревьев
        node->left = DeserializePreOrder(elements);
        node->right = DeserializePreOrder(elements);
        
        return node;
    }
    catch (...) {
        throw TreeException("Invalid node data: " + token);
    }
}

template <typename T>
Node<T>* BinaryTree<T>::DeserializeReversePreOrder(std::queue<std::string>& elements) {
    if (elements.empty()) return nullptr;
    
    std::string token = elements.front();
    elements.pop();
    
    if (token == "null") return nullptr;
    
    try {
        T value;
        std::istringstream(token) >> value;
        Node<T>* node = new Node<T>(value);
        
        // Сначала правое, затем левое поддерево
        node->right = DeserializeReversePreOrder(elements);
        node->left = DeserializeReversePreOrder(elements);
        
        return node;
    }
    catch (...) {
        throw TreeException("Invalid node data: " + token);
    }
}

template <typename T>
Node<T>* BinaryTree<T>::DeserializeInOrder(std::queue<std::string>& elements) {
    // InOrder десериализация требует дополнительной информации
    // В реальных проектах обычно используется комбинация InOrder+PreOrder
    throw TreeException("InOrder deserialization not supported alone");
}

template <typename T>
Node<T>* BinaryTree<T>::DeserializeReverseInOrder(std::queue<std::string>& elements) {
    // ReverseInOrder десериализация требует дополнительной информации
    // В реальных проектах обычно используется комбинация ReverseInOrder+PreOrder
    throw TreeException("ReverseInOrder deserialization not supported alone");
}

// Десериализация дерева из PostOrder представления
template <typename T>
Node<T>* BinaryTree<T>::DeserializePostOrder(std::queue<std::string>& elements) {
    std::stack<Node<T>*> nodeStack;
    
    while (!elements.empty()) {
        std::string token = elements.front();
        elements.pop();
        
        if (token == "null") {
            nodeStack.push(nullptr);
        }
        else {
            try {
                T value;
                std::istringstream(token) >> value;
                Node<T>* node = new Node<T>(value);
                
                // Для PostOrder правый потомок идет первым в стеке
                node->right = nodeStack.top();
                nodeStack.pop();
                node->left = nodeStack.top();
                nodeStack.pop();
                
                nodeStack.push(node);
            }
            catch (...) {
                // Очистка стека перед выбрасыванием исключения
                while (!nodeStack.empty()) {
                    delete nodeStack.top();
                    nodeStack.pop();
                }
                throw TreeException("Invalid node data: " + token);
            }
        }
    }
    
    if (nodeStack.size() != 1) {
        throw TreeException("Invalid PostOrder data format");
    }
    
    return nodeStack.top();
}

template <typename T>
Node<T>* BinaryTree<T>::DeserializeReversePostOrder(std::queue<std::string>& elements) {
    std::stack<Node<T>*> nodeStack;
    
    while (!elements.empty()) {
        std::string token = elements.front();
        elements.pop();
        
        if (token == "null") {
            nodeStack.push(nullptr);
        }
        else {
            try {
                T value;
                std::istringstream(token) >> value;
                Node<T>* node = new Node<T>(value);
                
                // Для ReversePostOrder сначала левый потомок (так как порядок обратный)
                node->left = nodeStack.top();
                nodeStack.pop();
                node->right = nodeStack.top();
                nodeStack.pop();
                
                nodeStack.push(node);
            }
            catch (...) {
                // Очистка стека при ошибке
                while (!nodeStack.empty()) {
                    delete nodeStack.top();
                    nodeStack.pop();
                }
                throw TreeException("Invalid node data: " + token);
            }
        }
    }
    
    if (nodeStack.size() != 1) {
        // Очистка памяти при неверном формате
        while (!nodeStack.empty()) {
            delete nodeStack.top();
            nodeStack.pop();
        }
        throw TreeException("Invalid ReversePostOrder sequence");
    }
    
    return nodeStack.top();
}


// Явное инстанцирование шаблонов для нужных типов
template class BinaryTree<int>;
template class BinaryTree<float>;
template class BinaryTree<std::string>;
template class BinaryTree<double>;
template class BinaryTree<std::complex<double>>;
