#include "model.h"

namespace s21 {

void Model::SetConfiguration(const Configuration& configuration) {
  if (configuration.GetNetworkType() != configuration_.GetNetworkType() &&
      network_) {
    auto weights = network_->GetWeights();
    network_ = std::make_unique<NeuralNetwork>(configuration.GetNetworkType(),
                                               network_->GetSettings());
    network_->SetWeights(weights);
  }
  configuration_ = configuration;
}

void Model::SetWeights(
    const std::string& filename,
    std::function<void(NetworkSettings, std::size_t, std::size_t)>
        success_callback,
    std::function<void(const std::string&)> error_callback) {
  try {
    auto data = WeightReader::Read(filename);

    network_ = std::make_unique<NeuralNetwork>(configuration_.GetNetworkType(),
                                               data.settings);
    network_->SetWeights(data.weights);

    if (success_callback)
      success_callback(network_->GetSettings(), data.epoch, data.accuracy);
  } catch (const std::runtime_error& e) {
    if (error_callback) error_callback(e.what());
  }
}

void Model::SetTrainDataset(
    const std::string& filename,
    std::function<void(std::string, std::size_t)> success_callback,
    std::function<void(const std::string&)> error_callback) {
  if (train_dataset_filename_ != filename) {
    std::thread([this, filename, success_callback, error_callback]() -> void {
      try {
        train_dataset_ = reader_->Read(filename);
        NormalizeData(&train_dataset_);
        train_dataset_filename_ = filename;

        if (success_callback) success_callback(filename, train_dataset_.size());
      } catch (const std::runtime_error& e) {
        if (error_callback) error_callback(e.what());
      }
    }).detach();
  } else {
    success_callback(filename, train_dataset_.size());
  }
}

void Model::SetTestDataset(
    const std::string& filename,
    std::function<void(std::string, std::size_t)> success_callback,
    std::function<void(const std::string&)> error_callback) {
  if (test_dataset_filename_ != filename) {
    std::thread([this, filename, success_callback, error_callback]() -> void {
      try {
        test_dataset_ = reader_->Read(filename);
        NormalizeData(&test_dataset_);
        test_dataset_filename_ = filename;

        if (success_callback) success_callback(filename, test_dataset_.size());
      } catch (const std::runtime_error& e) {
        if (error_callback) error_callback(e.what());
      }
    }).detach();
  } else {
    success_callback(filename, test_dataset_.size());
  }
}

void Model::Train(
    std::function<void()> start_callback,
    std::function<void(std::size_t)> epoch_progress_callback,
    std::function<void(std::size_t)> epoch_end_callback,
    std::function<void()> test_start_callback,
    std::function<void(std::size_t)> test_progress_callback,
    std::function<void(NetworkTestMetrics, std::size_t)> test_end_callback,
    std::function<void()> end_callback) {
  if (train_dataset_.size() == 0)
    throw std::runtime_error("тренировочный набор данных отсутствует");
  if (test_dataset_.size() == 0)
    throw std::runtime_error("тестовый набор данных отсутствует");

  train_exit_flag_.exchange(false);
  std::thread th([this, start_callback, epoch_progress_callback,
                  epoch_end_callback, test_start_callback,
                  test_progress_callback, test_end_callback,
                  end_callback]() -> void {
    NetworkSettings settings;
    settings.number_of_hidden_layers = configuration_.GetNumberOfHiddenLayers();
    network_ = std::make_unique<NeuralNetwork>(configuration_.GetNetworkType(),
                                               settings);

    network_->Train(
        train_dataset_, configuration_.GetEpochs(),
        configuration_.GetLearningRate(), start_callback,
        epoch_progress_callback,
        [this, test_start_callback, test_progress_callback, test_end_callback,
         epoch_end_callback](std::size_t epoch) -> void {
          if (train_exit_flag_ == false) {
            auto metrics = network_->Test(
                test_dataset_, 1, test_start_callback, test_progress_callback,
                [test_end_callback,
                 epoch](NetworkTestMetrics metrics_) -> void {
                  if (test_end_callback) test_end_callback(metrics_, epoch);
                },
                train_exit_flag_);

            if (configuration_.GetSaveWeightsEachEpoch()) {
              WeightWriter::Write(network_->GetWeights(),
                                  network_->GetSettings(), epoch,
                                  metrics.accuracy_percent);
            }

            if (epoch_end_callback) epoch_end_callback(epoch);
          }
        },
        end_callback, train_exit_flag_);
  });
  th.detach();
}

void Model::StopTrain() { train_exit_flag_.exchange(true); }

void Model::Test(double part, std::function<void()> start_callback,
                 std::function<void(std::size_t)> progress_callback,
                 std::function<void(NetworkTestMetrics)> end_callback) {
  if (!network_) throw std::runtime_error("веса сети отсутствуют");
  if (test_dataset_.size() == 0)
    throw std::runtime_error("тестовый набор данных отсутствует");

  test_exit_flag_.exchange(false);
  std::thread th(
      [this, part, start_callback, progress_callback, end_callback]() -> void {
        network_->Test(test_dataset_, part, start_callback, progress_callback,
                       end_callback, test_exit_flag_);
      });
  th.detach();
}

void Model::StopTest() { test_exit_flag_.exchange(true); }

void Model::TrainCrossValidation(
    std::size_t epochs, std::size_t k, std::function<void()> start_callback,
    std::function<void(std::size_t)> progress_callback,
    std::function<void(NetworkTestMetrics)> end_callback) {
  if (train_dataset_.size() == 0)
    throw std::runtime_error("тренировочный набор данных отсутствует");
  // if (test_dataset_.size() == 0)
  //   throw std::runtime_error("тестовый набор данных отсутствует");

  test_exit_flag_.exchange(false);
  std::thread th([this, epochs, k, start_callback, progress_callback,
                  end_callback]() {
    if (start_callback) start_callback();

    std::unique_ptr<NeuralNetwork> network_cv;
    std::unique_ptr<NeuralNetwork> network_best;
    NetworkTestMetrics best_metrics;

    NetworkType type = configuration_.GetNetworkType();
    NetworkSettings settings;
    settings.number_of_hidden_layers = configuration_.GetNumberOfHiddenLayers();

    size_t block_size = train_dataset_.size() / k;
    // std::cout << block_size << std::endl;

    // std::size_t prev_progress = std::string::npos;
    for (std::size_t i = 0; i < k && !test_exit_flag_; i++) {
      std::list<Image> test_data;
      test_data.splice(test_data.begin(), train_dataset_,
                       train_dataset_.begin(),
                       std::next(train_dataset_.begin(), block_size));
      network_cv = std::make_unique<NeuralNetwork>(type, settings);
      network_cv->Train(train_dataset_, epochs,
                        configuration_.GetLearningRate(), nullptr,
                        progress_callback, nullptr, nullptr, test_exit_flag_);
      NetworkTestMetrics metrics = network_cv->Test(
          test_data, 1, nullptr, progress_callback, nullptr, test_exit_flag_);

      if (metrics.fscore > best_metrics.fscore) {
        best_metrics = metrics;
        network_best = std::move(network_cv);
      }
      train_dataset_.splice(train_dataset_.end(), test_data);

      // std::size_t progress = static_cast<std::size_t>(
      //     static_cast<double>(i + 1) / static_cast<double>(k) * 100);
      // if (progress_callback && prev_progress != progress) {
      //   prev_progress = progress;
      //   progress_callback(progress);
      // }
    }

    network_ = std::move(network_best);

    if (end_callback) end_callback(best_metrics);
  });
  th.detach();
}

char Model::AnalyzeRawImage(const std::vector<double>& data) {
  char letter = 0;
  if (IsNetworkCreated()) {
    auto image = Image(data);
    image.NormalizeData();
    letter = static_cast<char>(Predict(image).first);
  }
  return static_cast<char>(letter) + 'A';
}

void Model::NormalizeData(std::list<Image>* images) {
  std::for_each(images->begin(), images->end(),
                [](Image& image) -> void { image.NormalizeData(); });
}

}  // namespace s21
