#ifndef SRC_MODEL_NEURAL_NETWORK_GRAPH_NETWORK_NEURON_H_
#define SRC_MODEL_NEURAL_NETWORK_GRAPH_NETWORK_NEURON_H_

#include <algorithm>
#include <map>
#include <vector>

#include "../utility.h"

namespace s21 {

class Neuron {
 public:
  Neuron() = default;
  explicit Neuron(double out);
  explicit Neuron(const std::map<Neuron*, double>& connections);
  explicit Neuron(std::vector<Neuron>& neurons);
  ~Neuron() = default;

  void SetOutput(double out);
  double GetOutput() const;
  void SetConnections(const std::map<Neuron*, double>& connections);
  std::map<Neuron*, double>& GetConnections();
  const std::map<Neuron*, double>& GetConnections() const;

  void AddRandomWeight(Neuron* neuron);
  void CalcOutput();

 private:
  double Delta(double error);
  double ActivationFunc(double x);

  double bias_ = 0;
  double output_ = 0;
  std::map<Neuron*, double> connections_;

  friend class Layer;
};

}  // namespace s21

#endif  // SRC_MODEL_NEURAL_NETWORK_GRAPH_NETWORK_NEURON_H_
