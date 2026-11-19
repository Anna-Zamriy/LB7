#include "NumericalDiff.h"
#include "DiffExceptions.h"
#include <cmath>
#include <stdexcept>
#include <string>

namespace NumericalDiff {
    namespace {
        // Допоміжна перевірка передумов перед обчисленням
        void validateInputs(MathFunc func, double x, double h) {
            if (func == nullptr) {
                throw std::invalid_argument("Function pointer must not be null");
            }
            if (!std::isfinite(x)) {
                throw std::invalid_argument("Point x must be finite");
            }
            if (!std::isfinite(h) || h <= 0.0) {
                throw InvalidStepException(h);
            }
        }

        // Безпечне обчислення значення функції в точці з перевіркою області визначення
        double evaluateSafe(MathFunc func, double point) {
            if (!std::isfinite(point)) {
                throw std::overflow_error("Shifted evaluation point overflowed");
            }
            const double val = func(point);
            if (std::isnan(val)) {
                throw DomainErrorException(point);
            }
            if (!std::isfinite(val)) {
                throw DerivativeOverflowException("function evaluation (Inf)");
            }
            return val;
        }

        // Перевірка скінченності фінального результату похідної
        double validateResult(double result, const std::string& methodName) {
            if (std::isnan(result)) {
                throw DomainErrorException(result);
            }
            if (!std::isfinite(result)) {
                throw DerivativeOverflowException(methodName);
            }
            return result;
        }
    }

    // 1. Правобічна різниця: f'(x) ≈ (f(x+h) - f(x)) / h
    double forwardDifference(MathFunc func, double x, double h) {
        validateInputs(func, x, h);
        const double fx = evaluateSafe(func, x);
        const double fxh = evaluateSafe(func, x + h);
        return validateResult((fxh - fx) / h, "forwardDifference");
    }

    // 2. Лівобічна різниця: f'(x) ≈ (f(x) - f(x-h)) / h
    double backwardDifference(MathFunc func, double x, double h) {
        validateInputs(func, x, h);
        const double fx = evaluateSafe(func, x);
        const double fxmh = evaluateSafe(func, x - h);
        return validateResult((fx - fxmh) / h, "backwardDifference");
    }

    // 3. Центральна різниця: f'(x) ≈ (f(x+h) - f(x-h)) / (2*h)
    double centralDifference(MathFunc func, double x, double h) {
        validateInputs(func, x, h);
        const double fxh = evaluateSafe(func, x + h);
        const double fxmh = evaluateSafe(func, x - h);
        return validateResult((fxh - fxmh) / (2.0 * h), "centralDifference");
    }

    // 4. Друга похідна: f''(x) ≈ (f(x+h) - 2*f(x) + f(x-h)) / (h^2)
    double secondDerivative(MathFunc func, double x, double h) {
        validateInputs(func, x, h);
        const double fxh = evaluateSafe(func, x + h);
        const double fx = evaluateSafe(func, x);
        const double fxmh = evaluateSafe(func, x - h);
        return validateResult((fxh - 2.0 * fx + fxmh) / (h * h), "secondDerivative");
    }
}