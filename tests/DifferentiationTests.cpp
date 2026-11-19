#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include <stdexcept>
#include "NumericalDiff.h"
#include "DifferentiationTask.h"
#include "DiffExceptions.h"

namespace {
    double cubic(double x) {
        return x * x * x; // f'(2) = 12, f''(2) = 12
    }

    double square(double x) {
        return x * x; // f'(3) = 6, f''(3) = 2
    }

    double safeSqrt(double x) {
        return std::sqrt(x); // NaN при x < 0
    }
}

// 1. Перевірка точності обчислень із заданою похибкою (EXPECT_NEAR)
TEST(NumericalDiffTest, ComputesDerivativesWithTolerance) {
    const double x = 2.0;
    const double h = 0.0001;

    EXPECT_NEAR(NumericalDiff::forwardDifference(cubic, x, h), 12.0, 1e-2);
    EXPECT_NEAR(NumericalDiff::backwardDifference(cubic, x, h), 12.0, 1e-2);
    EXPECT_NEAR(NumericalDiff::centralDifference(cubic, x, h), 12.0, 1e-6);
    EXPECT_NEAR(NumericalDiff::secondDerivative(cubic, x, h), 12.0, 1e-4);
}

// 2. Сценарій помилки 1: некоректний крок (h <= 0 або Inf/NaN)
TEST(NumericalDiffTest, RejectsInvalidStep) {
    EXPECT_THROW(NumericalDiff::forwardDifference(cubic, 2.0, 0.0), InvalidStepException);
    EXPECT_THROW(NumericalDiff::centralDifference(cubic, 2.0, -0.1), InvalidStepException);

    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();
    EXPECT_THROW(DifferentiationTask(2.0, inf), InvalidStepException);
    EXPECT_THROW(DifferentiationTask(2.0, nan), InvalidStepException);
}

// 3. Сценарій помилки 2: nullptr функції або нескінченний x
TEST(NumericalDiffTest, RejectsNullFunctionOrNonFiniteX) {
    EXPECT_THROW(NumericalDiff::forwardDifference(nullptr, 2.0, 0.01), std::invalid_argument);

    const double nan = std::numeric_limits<double>::quiet_NaN();
    EXPECT_THROW(DifferentiationTask(nan, 0.01), std::invalid_argument);
}

// 4. Сценарій помилки 3: вихід за область визначення (NaN у результаті функції)
TEST(NumericalDiffTest, ThrowsOnDomainError) {
    EXPECT_THROW(NumericalDiff::forwardDifference(safeSqrt, -1.0, 0.01), DomainErrorException);
    // У точці x = 0.005 ліва різниця звертається до x - h = -0.005 -> NaN
    EXPECT_THROW(NumericalDiff::backwardDifference(safeSqrt, 0.005, 0.01), DomainErrorException);
}

// 5. Сценарій помилки 4: переповнення похідної (Inf)
TEST(NumericalDiffTest, ThrowsOnDerivativeOverflow) {
    // Для великих значень x = 1e200 та надмалого h = 1e-200 ділення дасть Inf
    EXPECT_THROW(
        NumericalDiff::secondDerivative(cubic, 1e150, 1e-100),
        DerivativeOverflowException
    );
}

// 6. Сценарій помилки 5: звернення до результатів до виклику computeAll()
TEST(DifferentiationTaskTest, ThrowsWhenAccessingUncomputedResults) {
    DifferentiationTask task(2.0, 0.01);
    EXPECT_FALSE(task.hasResults());
    EXPECT_THROW(task.centralResult(), std::logic_error);
}

// 7. Перевірка сильної гарантії безпеки (Strong Guarantee)
TEST(DifferentiationTaskTest, ComputeAllPreservesStateOnException) {
    // Крок 1: успішно обчислюємо результати для f(x) = x^2 у точці x = 3
    DifferentiationTask task(3.0, 0.001);
    task.computeAll(square);

    ASSERT_TRUE(task.hasResults());
    const double savedForward = task.forwardResult();
    const double savedCentral = task.centralResult();
    const double savedSecond = task.secondDerivativeResult();

    // Крок 2: викликаємо computeAll з функцією, що викличе DomainErrorException (sqrt(-3))
    // Спочатку змінюємо x на -3 через новий об'єкт або передаємо функцію, що дає NaN у x = 3
    auto brokenAtThree = [](double x) -> double {
        return std::sqrt(x - 10.0); // NaN при x = 3
        };

    EXPECT_THROW(task.computeAll(brokenAtThree), DomainErrorException);

    // Перевіряємо, що після винятку всі попередні результати збережені без змін
    ASSERT_TRUE(task.hasResults());
    EXPECT_DOUBLE_EQ(task.forwardResult(), savedForward);
    EXPECT_DOUBLE_EQ(task.centralResult(), savedCentral);
    EXPECT_DOUBLE_EQ(task.secondDerivativeResult(), savedSecond);
}

// 8. Демонстрація порушення сильної гарантії (тест виявляє дефект в Unsafe-методі)
TEST(DifferentiationTaskTest, DISABLED_UnsafeComputeViolatesStrongGuarantee) {
    DifferentiationTask task(1e150, 1e-50);
    // Спочатку задамо валідні початкові результати для лінійної функції f(x) = x
    auto linear = [](double x) -> double { return x; };
    task.computeAll(linear);
    const double initialForward = task.forwardResult();

    // Тепер запускаємо computeAllUnsafe для f(x) = x^2:
    // forward/backward/central встигнуть перезаписати поля, а secondDerivative впаде з Overflow!
    EXPECT_THROW(task.computeAllUnsafe(square), DerivativeOverflowException);

    // Очікуємо незмінність стану (але unsafe-метод його вже зіпсував -> тест впаде, виявивши баг)
    EXPECT_DOUBLE_EQ(task.forwardResult(), initialForward);
}