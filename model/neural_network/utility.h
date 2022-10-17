#ifndef SRC_MODEL_NEURAL_NETWORK_UTILITY_H_
#define SRC_MODEL_NEURAL_NETWORK_UTILITY_H_

#include <cmath>
#include <random>

namespace s21::utility {

double DerivativeActivFunc(double x);

double ActivationFunc(double x);

double RandomWeight();

}  // namespace s21::utility

#endif  // SRC_MODEL_NEURAL_NETWORK_UTILITY_H_
