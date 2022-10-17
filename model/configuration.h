#ifndef SRC_MODEL_CONFIGURATION_H_
#define SRC_MODEL_CONFIGURATION_H_

#include <string>

#include "neural_network/network_interface.h"

namespace s21 {

class Configuration {
 public:
  enum class TestType { kWeight, kCrossValidation };

  Configuration() = default;

  NetworkType GetNetworkType() const { return network_type_; }
  void SetNetworkType(NetworkType type) { network_type_ = type; }

  std::size_t GetNumberOfHiddenLayers() const {
    return number_of_hidden_layers_;
  }
  void SetNumberOfHiddenLayers(std::size_t layers) {
    number_of_hidden_layers_ = layers;
  }

  TestType GetTestType() const { return test_type_; }
  void SetTestType(TestType test_type) { test_type_ = test_type; }

  double GetSelectionPart() const { return selection_part_; }
  void SetSelectionPart(double selection_part) {
    selection_part_ = selection_part;
  }

  std::size_t GetNumberOfGroups() const { return number_of_groups_; }
  void SetNumberOfGroups(std::size_t number_of_groups) {
    number_of_groups_ = number_of_groups;
  }

  std::size_t GetEpochs() const { return epochs_; }
  void SetEpochs(std::size_t epochs) { epochs_ = epochs; }

  bool GetSaveWeightsEachEpoch() const { return save_weights_each_epoch_; }
  void SetSaveWeightsEachEpoch(bool value) { save_weights_each_epoch_ = value; }

  double GetLearningRate() const { return learning_rate_; }
  void SetLearningRate(double learning_rate) { learning_rate_ = learning_rate; }

 private:
  NetworkType network_type_ = NetworkType::kMatrix;
  std::size_t number_of_hidden_layers_ = 4;

  TestType test_type_ = TestType::kWeight;
  double selection_part_ = 1;
  std::size_t number_of_groups_ = 1;

  std::size_t epochs_ = 3;
  bool save_weights_each_epoch_ = true;
  double learning_rate_ = 0.15;
};

}  // namespace s21

#endif  // SRC_MODEL_CONFIGURATION_H_
