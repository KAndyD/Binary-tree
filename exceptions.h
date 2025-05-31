#include <iostream>

#ifndef BINARY_TREE_EXCEPTIONS_H
#define BINARY_TREE_EXCEPTIONS_H

#include <stdexcept>
#include <string>
#include <complex>

// Base exception class
class TreeException : public std::runtime_error {
// public: - все члены класса доступны извне
public:
    // explicit запрещает неявное преобразование строки к исключению
    explicit TreeException(const std::string& message) : std::runtime_error(message) {}
    // Конструктор принимает сообщение об ошибке и передает его в базовый класс

};

// Специализированное исключение для некорректных операций с деревом (Наследуется от TreeException)
class InvalidTreeOperation : public TreeException {
public:
    explicit InvalidTreeOperation(const std::string& message) : TreeException("Invalid tree operation: " + message) {}
};

// Исключение для случаев, когда узел не найден
// Используется при поиске несуществующего узла
class NodeNotFound : public TreeException {
public:
    explicit NodeNotFound(const std::string& message) : TreeException("Node not found: " + message) {}

};

// Исключение для ошибок сериализации/десериализации
// Возникает при проблемах с преобразованием дерева в строку или обратно
class SerializationError : public TreeException {
public:
    explicit SerializationError(const std::string& message) : TreeException("Serialization error: " + message) {}
};

#endif