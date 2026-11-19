#include "DifferentiationTask.h"
#include "DiffExceptions.h"
#include <cmath>
#include <stdexcept>

void DifferentiationTask::validateParams(double x, double h) {
    if (!std::isfinite(x)) {
        throw std::invalid_argument("Task point x must be finite");
    }
    if (!std::isfinite(h) || h <= 0.0) {
        throw InvalidStepException(h);
    }
}

DifferentiationTask::DifferentiationTask(double x, double h)
    : x_(0.0), h_(1.0),
    forward_(0.0), backward_(0.0), central_(0.0), second_(0.0),
    hasResults_(false) {
    validateParams(x, h);
    x_ = x;
    h_ = h;
}

double DifferentiationTask::x() const noexcept { return x_; }
double DifferentiationTask::h() const noexcept { return h_; }
bool DifferentiationTask::hasResults() const noexcept { return hasResults_; }

double DifferentiationTask::forwardResult() const {
    if (!hasResults_) {
        throw std::logic_error("Derivatives have not been computed yet");
    }
    return forward_;
}

double DifferentiationTask::backwardResult() const {
    if (!hasResults_) {
        throw std::logic_error("Derivatives have not been computed yet");
    }
    return backward_;
}

double DifferentiationTask::centralResult() const {
    if (!hasResults_) {
        throw std::logic_error("Derivatives have not been computed yet");
    }
    return central_;
}

double DifferentiationTask::secondDerivativeResult() const {
    if (!hasResults_) {
        throw std::logic_error("Derivatives have not been computed yet");
    }
    return second_;
}

// Strong guarantee
void DifferentiationTask::setParameters(double x, double h) {
    validateParams(x, h);
    x_ = x;
    h_ = h;
    hasResults_ = false;
}

// Strong guarantee (prepare-then-commit)
void DifferentiationTask::computeAll(NumericalDiff::MathFunc func) {
    // 1. Підготовка нового стану у тимчасових змінних
    const double tmpForward = NumericalDiff::forwardDifference(func, x_, h_);
    const double tmpBackward = NumericalDiff::backwardDifference(func, x_, h_);
    const double tmpCentral = NumericalDiff::centralDifference(func, x_, h_);
    const double tmpSecond = NumericalDiff::secondDerivative(func, x_, h_);

    // 2. Фіксація змін (присвоєння double та bool не генерує винятків)
    forward_ = tmpForward;
    backward_ = tmpBackward;
    central_ = tmpCentral;
    second_ = tmpSecond;
    hasResults_ = true;
}

// Навмисно небезпечна реалізація (для демонстрації порушення сильної гарантії)
void DifferentiationTask::computeAllUnsafe(NumericalDiff::MathFunc func) {
    // Помилка: змінюємо поля об'єкта одразу, до завершення всіх обчислень
    forward_ = NumericalDiff::forwardDifference(func, x_, h_);
    backward_ = NumericalDiff::backwardDifference(func, x_, h_);
    central_ = NumericalDiff::centralDifference(func, x_, h_);
    // Якщо переповнення станеться тут (ділення на h*h), попередні поля вже перезаписані!
    second_ = NumericalDiff::secondDerivative(func, x_, h_);
    hasResults_ = true;
}