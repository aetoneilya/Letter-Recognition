#ifndef SRC_MODEL_NEURAL_NETWORK_GRAPH_NETWORK_LAYER_H_
#define SRC_MODEL_NEURAL_NETWORK_GRAPH_NETWORK_LAYER_H_

#include <memory>

#include "neuron.h"

namespace s21 {

enum class LayerType { kInput = 0, kHidden, kOutput };
class Layer {
 public:
  explicit Layer(unsigned long number_of_neurons);
  Layer(unsigned long number_of_neurons,
        const std::unique_ptr<Layer>& prev_layer);
  ~Layer() = default;

  LayerType GetLayerType();

  std::vector<Neuron>& Neurons();

  void SetOutput(const std::vector<double>& outputs);
  void CalculateOutput();

  std::vector<double> AdjustWeights(double learning_rate,
                                    std::vector<double> errors);
  std::vector<double> Error(const std::vector<double>& expected_output);

 private:
  void SetLayerType(LayerType type);
  void CalculateDelta();

  LayerType type_;
  std::vector<Neuron> neurons_;
};

}  // namespace s21

#endif  //  SRC_MODEL_NEURAL_NETWORK_GRAPH_NETWORK_LAYER_H_
