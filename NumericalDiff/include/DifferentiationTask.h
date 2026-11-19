#pragma once
#include "NumericalDiff.h"

class DifferentiationTask {
private:
    double x_;
    double h_;
    double forward_;
    double backward_;
    double central_;
    double second_;
    bool hasResults_;

    static void validateParams(double x, double h);

public:
    // Конструктор (немає конструктора без параметрів — захист інваріантів)
    DifferentiationTask(double x, double h);

    // Геттери з гарантією no-throw
    double x() const noexcept;
    double h() const noexcept;
    bool hasResults() const noexcept;

    // Доступ до обчислених результатів (кидає std::logic_error, якщо ще не обчислено)
    double forwardResult() const;
    double backwardResult() const;
    double centralResult() const;
    double secondDerivativeResult() const;

    // Strong guarantee: оновлює параметри та скидає результати лише після перевірки
    void setParameters(double x, double h);

    // Strong guarantee (prepare-then-commit): обчислює всі 4 похідні;
    // якщо хоч одна впаде з винятком — старі результати НЕ змінюються
    void computeAll(NumericalDiff::MathFunc func);

    // Навмисно некоректна реалізація (порушує сильну гарантію: змінює стан покроково)
    void computeAllUnsafe(NumericalDiff::MathFunc func);
};