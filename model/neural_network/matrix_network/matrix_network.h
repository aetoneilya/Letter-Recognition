#ifndef SRC_MODEL_NEURAL_NETWORK_MATRIX_NETWORK_MATRIX_NETWORK_H_
#define SRC_MODEL_NEURAL_NETWORK_MATRIX_NETWORK_MATRIX_NETWORK_H_

#include "../../../lib/matrixplus/s21_matrix_oop.h"
#include "../network_interface.h"
#include "../utility.h"

namespace s21 {

class MatrixNetwork : public NetworkInterface {
 public:
  explicit MatrixNetwork(NetworkSettings settings);
  ~MatrixNetwork() = default;

  void SetInput(const std::vector<double>& outputs) override;
  void ForwardPropagation() override;
  void BackPropagation(const std::vector<double>& expected_output,
                       double learning_rate_) override;
  std::vector<double> GetOutput() override;

  std::vector<double> GetWeights() override;
  void LoadWeights(const std::vector<double>& weights) override;

 private:
  void FillMatrixRandom(Matrix& m);
  Matrix ActivationFuncMatrix(const Matrix& m);
  Matrix DerivativeActivationFuncMatrix(const Matrix& m);
  void AdjustWeights(size_t i, double learning_rate, const Matrix& error);

  std::vector<Matrix> values_;
  std::vector<Matrix> weights_;
};

}  // namespace s21

#endif  // SRC_MODEL_NEURAL_NETWORK_MATRIX_NETWORK_MATRIX_NETWORK_H_
