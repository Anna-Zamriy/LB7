#include <iostream>
#include <cmath>
#include <exception>
#include "DifferentiationTask.h"
#include "DiffExceptions.h"
#include "ScopeLogger.h"

double sampleFunction(double x) {
    return x * x * x; // f(x) = x^3 -> f'(2) = 12, f''(2) = 12
}

double sqrtFunction(double x) {
    return std::sqrt(x); // Не визначена для x < 0 (повертає NaN)
}

// Рівень 3 — виникнення винятку
void computeLevel3() {
    ScopeLogger logger("computeLevel3");
    std::cout << "Attempting to compute derivative of sqrt(x) at x = -4.0...\n";
    DifferentiationTask badTask(-4.0, 0.01);
    badTask.computeAll(sqrtFunction); // Згенерує DomainErrorException
}

// Рівень 2 — проміжне перехоплення, логування та повторне генерування (throw;)
void computeLevel2() {
    ScopeLogger logger("computeLevel2");
    try {
        computeLevel3();
    }
    catch (const DiffException& ex) {
        std::cout << "[LOG] computeLevel2 caught DiffException: " << ex.what() << '\n';
        throw; // Зберігаємо оригінальний тип винятку без slicing
    }
}

// Рівень 1 — виклик наступного рівня
void computeLevel1() {
    ScopeLogger logger("computeLevel1");
    computeLevel2();
}

int main() {
    std::cout << "=== 1. NORMAL EXECUTION & STRONG GUARANTEE ===\n";
    DifferentiationTask task(2.0, 0.0001);
    task.computeAll(sampleFunction);

    std::cout << "Function: f(x) = x^3 at x = 2.0\n";
    std::cout << "Forward Diff : " << task.forwardResult() << '\n';
    std::cout << "Backward Diff: " << task.backwardResult() << '\n';
    std::cout << "Central Diff : " << task.centralResult() << '\n';
    std::cout << "Second Deriv : " << task.secondDerivativeResult() << '\n';

    // Перевірка сильної гарантії при спробі передати некоректний крок
    try {
        std::cout << "\nTrying setParameters(2.0, -0.5)...\n";
        task.setParameters(2.0, -0.5);
    }
    catch (const InvalidStepException& ex) {
        std::cout << "Caught: " << ex.what() << " (step = " << ex.step() << ")\n";
        std::cout << "State preserved! Central Diff is still: " << task.centralResult() << '\n';
    }

    std::cout << "\n=== 2. STACK UNWINDING & RAII DEMO ===\n";
    try {
        computeLevel1();
    }
    catch (const DomainErrorException& ex) {
        std::cout << "[MAIN] Caught DomainErrorException: " << ex.what()
            << " at point x = " << ex.point() << '\n';
    }
    catch (const std::exception& ex) {
        std::cout << "[MAIN] Caught std::exception: " << ex.what() << '\n';
    }

    std::cout << "Program finished safely.\n";
    return 0;
}