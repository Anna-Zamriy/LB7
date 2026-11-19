#pragma once

namespace NumericalDiff {
    using MathFunc = double (*)(double);

    double forwardDifference(MathFunc func, double x, double h);
    double backwardDifference(MathFunc func, double x, double h);
    double centralDifference(MathFunc func, double x, double h);
    double secondDerivative(MathFunc func, double x, double h);
}