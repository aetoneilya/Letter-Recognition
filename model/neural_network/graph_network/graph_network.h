#ifndef SRC_MODEL_NEURAL_NETWORK_GRAPH_NETWORK_GRAPH_NETWORK_H_
#define SRC_MODEL_NEURAL_NETWORK_GRAPH_NETWORK_GRAPH_NETWORK_H_

#include <algorithm>
#include <memory>
#include <string>

#include "../network_interface.h"
#include "layer.h"

namespace s21 {

class GraphNetwork : public NetworkInterface {
 public:
  explicit GraphNetwork(NetworkSettings settings);
  ~GraphNetwork() = default;

  void SetInput(const std::vector<double>& outputs) override;
  void BackPropagation(const std::vector<double>& expected_output,
                       double learning_rate_) override;
  void ForwardPropagation() override;
  std::vector<double> GetOutput() override;

  std::vector<double> GetWeights() override;
  void LoadWeights(const std::vector<double>& weights) override;

 private:
  std::vector<std::unique_ptr<Layer>> layers_;
};

}  // namespace s21

#endif  // SRC_MODEL_NEURAL_NETWORK_GRAPH_NETWORK_GRAPH_NETWORK_H_
