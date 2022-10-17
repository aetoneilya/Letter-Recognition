#include "utility.h"

namespace s21::utility {

double DerivativeActivFunc(double x) { return x * (1.0 - x); }

double ActivationFunc(double x) { return 1.0 / (1.0 + exp(-x)); }

double RandomWeight() {
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<double> distr(-1, 1);

  return distr(eng);
}

}  // namespace s21::utility
