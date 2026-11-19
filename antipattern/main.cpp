#include <iostream>
#include <string>
#include <stdexcept>
#include <exception>

void throwInteger() {
    throw 42;
}

void throwString() {
    throw std::string("Invalid differentiation step");
}

void throwStandardException() {
    throw std::invalid_argument("Invalid differentiation step");
}

void demonstrateSlicing() {
    try {
        throw std::invalid_argument("Specific invalid_argument error");
    }
    catch (const std::exception& ex) {
        // АНТИПАТЕРН: копіювання за значенням базового класу (object slicing)
        throw ex;
    }
}

int main() {
    std::cout << "=== Example 1: int exception ===\n";
    try {
        throwInteger();
    }
    catch (const std::exception& ex) {
        std::cout << "std::exception: " << ex.what() << '\n';
    }
    catch (int error) {
        std::cout << "Caught int: " << error << " (missed by std::exception)\n";
    }

    std::cout << "\n=== Example 2: string exception ===\n";
    try {
        throwString();
    }
    catch (const std::exception& ex) {
        std::cout << "std::exception: " << ex.what() << '\n';
    }
    catch (const std::string& error) {
        std::cout << "Caught string: " << error << " (missed by std::exception)\n";
    }

    std::cout << "\n=== Example 3: standard exception ===\n";
    try {
        throwStandardException();
    }
    catch (const std::exception& ex) {
        std::cout << "Caught std::exception: " << ex.what() << '\n';
    }

    std::cout << "\n=== Example 4: object slicing ===\n";
    try {
        demonstrateSlicing();
    }
    catch (const std::invalid_argument& ex) {
        std::cout << "Caught invalid_argument: " << ex.what() << '\n';
    }
    catch (const std::exception& ex) {
        std::cout << "Caught base std::exception (sliced!): " << ex.what() << '\n';
    }

    return 0;
}