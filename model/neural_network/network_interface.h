#ifndef SRC_MODEL_NEURAL_NETWORK_NETWORK_INTERFACE_H_
#define SRC_MODEL_NEURAL_NETWORK_NETWORK_INTERFACE_H_

#include <vector>

namespace s21 {

enum class NetworkType { kMatrix, kGraph };

struct NetworkTestMetrics {
  double accuracy = 0;
  std::size_t accuracy_percent = 0;
  double precision = 0;
  double recall = 0;
  double fscore = 0;
  std::size_t time;
};

struct NetworkSettings {
  std::size_t number_of_hidden_layers = 4;
  std::size_t neurons_in_input_layer = 784;
  std::size_t neurons_in_hidden_layer = 140;
  std::size_t neurons_in_output_layer = 26;
};

class NetworkInterface {
 public:
  virtual ~NetworkInterface() = default;

  virtual void SetInput(const std::vector<double>& outputs) = 0;
  virtual void BackPropagation(const std::vector<double>& expected_output,
                               double learning_rate_) = 0;
  virtual void ForwardPropagation() = 0;
  virtual std::vector<double> GetOutput() = 0;
  virtual std::vector<double> GetWeights() = 0;
  virtual void LoadWeights(const std::vector<double>& weights) = 0;
};

}  // namespace s21

#endif  // SRC_MODEL_NEURAL_NETWORK_NETWORK_INTERFACE_H_
