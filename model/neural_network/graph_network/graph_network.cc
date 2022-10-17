#include "graph_network.h"

namespace s21 {

GraphNetwork::GraphNetwork(NetworkSettings settings) {
  layers_.push_back(std::make_unique<Layer>(settings.neurons_in_input_layer));

  for (std::size_t i = 0; i < settings.number_of_hidden_layers; i++) {
    layers_.push_back(std::make_unique<Layer>(settings.neurons_in_hidden_layer,
                                              layers_.back()));
  }

  layers_.push_back(std::make_unique<Layer>(settings.neurons_in_output_layer,
                                            layers_.back()));
}

void GraphNetwork::BackPropagation(const std::vector<double> &expected_output,
                                   double learning_rate_) {
  std::vector<double> error = layers_.back()->Error(expected_output);

  for (std::size_t layer = layers_.size() - 1; layer != 0; layer--) {
    error = layers_.at(layer)->AdjustWeights(learning_rate_, error);
  }
}

void GraphNetwork::SetInput(const std::vector<double> &outputs) {
  layers_.front()->SetOutput(outputs);
}

void GraphNetwork::ForwardPropagation() {
  for (auto &layer : layers_) {
    layer->CalculateOutput();
  }
}

std::vector<double> GraphNetwork::GetOutput() {
  std::vector<double> output;

  auto neurons = layers_.back()->Neurons();
  std::transform(neurons.begin(), neurons.end(), std::back_inserter(output),
                 [](const Neuron &n) { return n.GetOutput(); });

  return output;
}

std::vector<double> GraphNetwork::GetWeights() {
  std::vector<double> weights;

  for (const auto &l : layers_) {
    for (const auto &n : l->Neurons()) {
      std::for_each(n.GetConnections().begin(), n.GetConnections().end(),
                    [&weights](const std::pair<Neuron *, double> &el) -> void {
                      weights.push_back(el.second);
                    });
    }
  }

  return weights;
}

void GraphNetwork::LoadWeights(const std::vector<double> &weights) {
  std::size_t i = 0;
  for (auto &l : layers_)
    for (auto &n : l->Neurons())
      for (auto &p : n.GetConnections()) {
        p.second = weights[i++];
      }

  return;
}

}  // namespace s21
