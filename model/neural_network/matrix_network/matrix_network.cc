#include "matrix_network.h"

namespace s21 {

MatrixNetwork::MatrixNetwork(NetworkSettings settings)
    : values_(settings.number_of_hidden_layers + 2) {
  Matrix weight(settings.neurons_in_hidden_layer,
                settings.neurons_in_input_layer);

  FillMatrixRandom(weight);
  weights_.push_back(weight);

  for (std::size_t i = 0; i < settings.number_of_hidden_layers - 1; i++) {
    weight = Matrix(settings.neurons_in_hidden_layer,
                    settings.neurons_in_hidden_layer);
    FillMatrixRandom(weight);
    weights_.push_back(weight);
  }

  weight = Matrix(settings.neurons_in_output_layer,
                  settings.neurons_in_hidden_layer);
  FillMatrixRandom(weight);
  weights_.push_back(weight);
}

void MatrixNetwork::SetInput(const std::vector<double> &outputs) {
  values_.front() = Matrix(outputs);
}

void MatrixNetwork::ForwardPropagation() {
  for (size_t i = 0; i < weights_.size(); i++) {
    values_[i + 1] = ActivationFuncMatrix(weights_[i] * values_[i]);
  }
}

Matrix Mul(const Matrix &m1, const Matrix &m2) {
  if (m1.GetColumns() != m2.GetColumns() || m2.GetRows() != m1.GetRows())
    throw std::logic_error("Mul: invalid matrix dims");

  Matrix res(m1.GetRows(), m1.GetColumns());
  for (size_t i = 0; i < m1.GetRows(); i++)
    for (size_t j = 0; j < m1.GetColumns(); j++) {
      res(i, j) = m1(i, j) * m2(i, j);
    }
  return res;
}

void MatrixNetwork::BackPropagation(const std::vector<double> &expected_output,
                                    double learning_rate_) {
  Matrix error = values_.back() - Matrix(expected_output);
  error = Mul(error, DerivativeActivationFuncMatrix(values_.back()));
  AdjustWeights(weights_.size() - 1, learning_rate_, error);

  for (int i = (int)weights_.size() - 2; i >= 0; i--) {
    error = Mul(weights_[i + 1].Transpose() * error,
                DerivativeActivationFuncMatrix(values_[i + 1]));
    AdjustWeights(i, learning_rate_, error);
  }
}

void MatrixNetwork::AdjustWeights(size_t weight_ind, double learning_rate,
                                  const Matrix &error) {
  weights_[weight_ind] -=
      learning_rate * error * values_[weight_ind].Transpose();
}

std::vector<double> MatrixNetwork::GetOutput() {
  std::vector<double> result(values_.back().GetRows(), 0);

  for (size_t i = 0; i < values_.back().GetRows(); i++) {
    result[i] = values_.back()(i, 0);
  }

  return result;
}

std::vector<double> MatrixNetwork::GetWeights() {
  std::vector<double> weights;

  for (auto &matrix : weights_) {
    for (std::size_t row = 0; row < matrix.GetRows(); row++) {
      for (std::size_t column = 0; column < matrix.GetColumns(); column++) {
        weights.push_back(matrix(row, column));
      }
    }
  }

  return weights;
}

void MatrixNetwork::LoadWeights(const std::vector<double> &weights) {
  std::size_t i = 0;
  for (auto &matrix : weights_) {
    for (std::size_t row = 0; row < matrix.GetRows(); row++) {
      for (std::size_t column = 0; column < matrix.GetColumns(); column++) {
        matrix(row, column) = weights[i++];
      }
    }
  }
}

void MatrixNetwork::FillMatrixRandom(Matrix &m) {
  for (size_t i = 0; i < m.GetRows(); i++)
    for (size_t j = 0; j < m.GetColumns(); j++) {
      m(i, j) = utility::RandomWeight();
    }
}

Matrix MatrixNetwork::ActivationFuncMatrix(const Matrix &m) {
  Matrix res(m.GetRows(), m.GetColumns());
  for (size_t i = 0; i < m.GetRows(); i++)
    for (size_t j = 0; j < m.GetColumns(); j++) {
      res(i, j) = utility::ActivationFunc(m(i, j));
    }
  return res;
}

Matrix MatrixNetwork::DerivativeActivationFuncMatrix(const Matrix &m) {
  Matrix res(m.GetRows(), m.GetColumns());
  for (size_t i = 0; i < m.GetRows(); i++)
    for (size_t j = 0; j < m.GetColumns(); j++) {
      res(i, j) = utility::DerivativeActivFunc(m(i, j));
    }
  return res;
}

}  // namespace s21
