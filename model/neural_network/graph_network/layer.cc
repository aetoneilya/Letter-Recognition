#include "layer.h"

namespace s21 {

Layer::Layer(unsigned long number_of_neurons) : type_(LayerType::kInput) {
  for (std::size_t i = 0; i < number_of_neurons; i++) {
    neurons_.emplace_back(0);
  }
}

Layer::Layer(unsigned long number_of_neurons,
             const std::unique_ptr<Layer>& prev_layer)
    : type_(LayerType::kOutput) {
  if (prev_layer->type_ == LayerType::kOutput)
    prev_layer->SetLayerType(LayerType::kHidden);

  for (std::size_t i = 0; i < number_of_neurons; i++) {
    neurons_.emplace_back(prev_layer->Neurons());
  }
}

std::vector<Neuron>& Layer::Neurons() { return neurons_; }

LayerType Layer::GetLayerType() { return type_; }

void Layer::SetLayerType(LayerType type) { type_ = type; }

void Layer::CalculateOutput() {
  if (type_ == LayerType::kInput) return;

  for (Neuron& neuron : neurons_) {
    neuron.CalcOutput();
  }
}

std::vector<double> Layer::AdjustWeights(double learning_rate,
                                         std::vector<double> errors) {
  std::vector<double> input_errors(neurons_.at(0).connections_.size(), 0);
  for (std::size_t i = 0; i < neurons_.size(); i++) {
    double delta = neurons_.at(i).Delta(errors.at(i));
    double delta_coef = delta * learning_rate;

    int j = 0;
    for (auto& [prev_neuron, weight] : neurons_.at(i).connections_) {
      weight = weight - (prev_neuron->GetOutput() * delta_coef);
      input_errors.at(j) += delta * weight;
      j++;
    }
    neurons_.at(i).bias_ -= delta_coef;
  }

  return input_errors;
}

std::vector<double> Layer::Error(const std::vector<double>& expected_output) {
  std::vector<double> error;
  for (std::size_t i = 0; i < neurons_.size(); i++) {
    error.push_back(neurons_[i].GetOutput() - expected_output[i]);
  }
  return error;
}

void Layer::SetOutput(const std::vector<double>& outputs) {
  for (std::size_t i = 0; i < neurons_.size(); i++) {
    neurons_[i].SetOutput(outputs.at(i));
  }
}

}  // namespace s21
