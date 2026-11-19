#pragma once
#include <stdexcept>
#include <string>

// Базовий клас усіх користувацьких винятків бібліотеки NumericalDiff
class DiffException : public std::runtime_error {
public:
    explicit DiffException(const std::string& message)
        : std::runtime_error(message) {}
};

// 1. Помилка кроку диференціювання (h <= 0 або NaN/Inf)
class InvalidStepException : public DiffException {
private:
    double step_;
public:
    explicit InvalidStepException(double step)
        : DiffException("Invalid differentiation step: h must be finite and > 0"),
        step_(step) {
    }

    double step() const noexcept { return step_; }
};

// 2. Помилка області визначення (функція повернула NaN у заданій точці)
class DomainErrorException : public DiffException {
private:
    double point_;
public:
    explicit DomainErrorException(double point)
        : DiffException("Function is undefined (NaN) at evaluation point"),
        point_(point) {
    }

    double point() const noexcept { return point_; }
};

// 3. Переповнення або нескінченний результат похідної (Inf)
class DerivativeOverflowException : public DiffException {
private:
    std::string method_;
public:
    explicit DerivativeOverflowException(const std::string& method)
        : DiffException("Derivative calculation overflow in method: " + method),
        method_(method) {
    }

    const std::string& method() const noexcept { return method_; }
};