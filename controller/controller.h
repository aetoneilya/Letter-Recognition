#ifndef SRC_CONTROLLER_CONTROLLER_H_
#define SRC_CONTROLLER_CONTROLLER_H_

#include <iostream>
#include <string>
#include <vector>

#include "../model/model.h"

namespace s21 {

class Controller {
 public:
  explicit Controller(Model* model) : model_(model) {}

  Configuration GetConfiguration() const { return model_->GetConfiguration(); }
  void SetConfiguration(const Configuration& configuration) {
    model_->SetConfiguration(configuration);
  }

  std::vector<double> GetWeights() const { return model_->GetWeights(); }

  void SetWeights(
      const std::string& filename,
      std::function<void(const NetworkSettings&, std::size_t, std::size_t)>
          success_callback = nullptr,
      std::function<void(const std::string&)> error_callback = nullptr) {
    model_->SetWeights(filename, success_callback, error_callback);
  }

  void SetTrainDataset(
      const std::string& filename,
      std::function<void(std::string, std::size_t)> success_callback = nullptr,
      std::function<void(const std::string&)> error_callback = nullptr) {
    model_->SetTrainDataset(filename, success_callback, error_callback);
  }

  void SetTestDataset(
      const std::string& filename,
      std::function<void(std::string, std::size_t)> success_callback = nullptr,
      std::function<void(const std::string&)> error_callback = nullptr) {
    model_->SetTestDataset(filename, success_callback, error_callback);
  }

  bool IsNetworkCreated() { return model_->IsNetworkCreated(); }

  const NetworkSettings& GetSettings() const { return model_->GetSettings(); }

  void Train(std::function<void()> start_callback = nullptr,
             std::function<void(std::size_t)> epoch_progress_callback = nullptr,
             std::function<void(std::size_t)> epoch_end_callback = nullptr,
             std::function<void()> test_start_callback = nullptr,
             std::function<void(std::size_t)> test_progress_callback = nullptr,
             std::function<void(NetworkTestMetrics, std::size_t)>
                 test_end_callback = nullptr,
             std::function<void()> end_callback = nullptr,
             std::function<void(const std::string&)> error_callback = nullptr) {
    try {
      model_->Train(start_callback, epoch_progress_callback, epoch_end_callback,
                    test_start_callback, test_progress_callback,
                    test_end_callback, end_callback);
    } catch (const std::runtime_error& e) {
      if (error_callback) error_callback(e.what());
    }
  }

  void StopTrain() { model_->StopTrain(); }

  void Test(std::function<void()> start_callback = nullptr,
            std::function<void(std::size_t)> progress_callback = nullptr,
            std::function<void(NetworkTestMetrics)> end_callback = nullptr,
            std::function<void(const std::string&)> error_callback = nullptr) {
    try {
      switch (GetConfiguration().GetTestType()) {
        case Configuration::TestType::kCrossValidation:
          model_->TrainCrossValidation(GetConfiguration().GetEpochs(),
                                       GetConfiguration().GetNumberOfGroups(),
                                       start_callback, progress_callback,
                                       end_callback);
          break;
        case Configuration::TestType::kWeight:
          model_->Test(GetConfiguration().GetSelectionPart(), start_callback,
                       progress_callback, end_callback);
          break;
      }
    } catch (const std::runtime_error& e) {
      if (error_callback) error_callback(e.what());
    }
  }

  void StopTest() { model_->StopTest(); }

  char AnalyzeRawImage(const std::vector<double>& data) {
    return model_->AnalyzeRawImage(data);
  }

 private:
  Model* model_;
};

}  // namespace s21

#endif  // SRC_CONTROLLER_CONTROLLER_H_
