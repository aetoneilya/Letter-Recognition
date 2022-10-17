#include "neuron.h"

namespace s21 {

Neuron::Neuron(double out) : output_(out) {}

Neuron::Neuron(const std::map<Neuron *, double> &connections)
    : connections_(connections) {}

Neuron::Neuron(std::vector<Neuron> &neurons) {
  for (Neuron &neuron : neurons) {
    AddRandomWeight(&neuron);
  }
}

void Neuron::SetOutput(double out) { output_ = out; }

double Neuron::GetOutput() const { return output_; }

void Neuron::SetConnections(const std::map<Neuron *, double> &connections) {
  connections_ = connections;
}

std::map<Neuron *, double> &Neuron::GetConnections() { return connections_; }

const std::map<Neuron *, double> &Neuron::GetConnections() const {
  return connections_;
}

void Neuron::AddRandomWeight(Neuron *neuron) {
  connections_[neuron] = utility::RandomWeight();
}

void Neuron::CalcOutput() {
  double out = bias_;
  for (auto [prev_neuron, weight] : connections_) {
    out += weight * prev_neuron->GetOutput();
  }
  output_ = ActivationFunc(out);
}

double Neuron::Delta(double error) {
  return (utility::DerivativeActivFunc(output_) * error);
}

double Neuron::ActivationFunc(double x) { return utility::ActivationFunc(x); }

}  // namespace s21
