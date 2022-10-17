#ifndef SRC_MODEL_MODEL_H_
#define SRC_MODEL_MODEL_H_

#include <atomic>
#include <iostream>
#include <memory>

#include "configuration.h"
#include "neural_network/io/weight_reader.h"
#include "neural_network/neural_network.h"
#include "reader/csv_reader.h"

namespace s21 {

class Model {
 public:
  Model() = default;

  Configuration GetConfiguration() const { return configuration_; }
  void SetConfiguration(const Configuration& configuration);

  const NetworkSettings& GetSettings() const { return network_->GetSettings(); }

  std::vector<double> GetWeights() const { return network_->GetWeights(); }

  void SetWeights(
      const std::string& filename,
      std::function<void(NetworkSettings, std::size_t, std::size_t)>
          success_callback = nullptr,
      std::function<void(const std::string&)> error_callback = nullptr);

  void SetTrainDataset(
      const std::string& filename,
      std::function<void(std::string, std::size_t)> success_callback = nullptr,
      std::function<void(const std::string&)> error_callback = nullptr);

  void SetTestDataset(
      const std::string& filename,
      std::function<void(std::string, std::size_t)> success_callback = nullptr,
      std::function<void(const std::string&)> error_callback = nullptr);

  bool IsNetworkCreated() { return network_ != nullptr; }

  void Train(std::function<void()> start_callback = nullptr,
             std::function<void(std::size_t)> epoch_progress_callback = nullptr,
             std::function<void(std::size_t)> epoch_end_callback = nullptr,
             std::function<void()> test_start_callback = nullptr,
             std::function<void(std::size_t)> test_progress_callback = nullptr,
             std::function<void(NetworkTestMetrics, std::size_t)>
                 test_end_callback = nullptr,
             std::function<void()> end_callback = nullptr);

  void StopTrain();

  void Test(double part, std::function<void()> start_callback = nullptr,
            std::function<void(std::size_t)> progress_callback = nullptr,
            std::function<void(NetworkTestMetrics)> end_callback = nullptr);

  void StopTest();

  void TrainCrossValidation(
      std::size_t epochs, std::size_t k,
      std::function<void()> start_callback = nullptr,
      std::function<void(std::size_t)> progress_callback = nullptr,
      std::function<void(NetworkTestMetrics)> end_callback = nullptr);

  std::pair<int, double> Predict(const Image& image) {
    return network_->Predict(image);
  }

  char AnalyzeRawImage(const std::vector<double>& data);

 private:
  void NormalizeData(std::list<Image>* images);

  std::unique_ptr<BaseFileReader> reader_ = std::make_unique<CsvReader>();

  std::string train_dataset_filename_;
  std::list<Image> train_dataset_;

  std::string test_dataset_filename_;
  std::list<Image> test_dataset_;

  Configuration configuration_;
  std::unique_ptr<NeuralNetwork> network_;
  std::atomic_bool train_exit_flag_;
  std::atomic_bool test_exit_flag_;
};

}  // namespace s21

#endif  // SRC_MODEL_MODEL_H_
