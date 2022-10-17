#ifndef SRC_MODEL_NEURAL_NETWORK_NEURAL_NETWORK_H_
#define SRC_MODEL_NEURAL_NETWORK_NEURAL_NETWORK_H_

#include <atomic>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <list>
#include <memory>
#include <sstream>
#include <vector>

#include "../image.h"
#include "../reader/csv_reader.h"
#include "graph_network/graph_network.h"
#include "io/weight_writer.h"
#include "matrix_network/matrix_network.h"
#include "network_interface.h"

namespace s21 {

class NeuralNetwork {
 public:
  NeuralNetwork(NetworkType type, NetworkSettings settings)
      : type_(type), settings_(settings) {
    switch (type) {
      case NetworkType::kMatrix:
        network_ = std::make_unique<MatrixNetwork>(settings);
        break;
      case NetworkType::kGraph:
        network_ = std::make_unique<GraphNetwork>(settings);
        break;
    }
  }

  void Train(const std::list<Image>& data, std::size_t epochs,
             double learning_rate = 0.15,
             std::function<void()> start_callback = nullptr,
             std::function<void(std::size_t)> epoch_progress_callback = nullptr,
             std::function<void(std::size_t)> epoch_end_callback = nullptr,
             std::function<void()> end_callback = nullptr,
             const std::atomic_bool& exit = std::atomic_bool(false));
  void TrainEpoch(const std::list<Image>& data, double learning_rate,
                  std::function<void(std::size_t)> epoch_progress_callback,
                  const std::atomic_bool& exit);

  NetworkTestMetrics Test(
      const std::list<Image>& data, double part,
      std::function<void()> start_callback = nullptr,
      std::function<void(std::size_t)> progress_callback = nullptr,
      std::function<void(NetworkTestMetrics)> end_callback = nullptr,
      const std::atomic_bool& exit = std::atomic_bool(false));

  std::vector<double> Prediction(const Image& image);
  std::pair<std::size_t, double> Predict(const Image& image);

  std::vector<double> GetWeights() const;
  void SetWeights(const std::vector<double>& weights);

  NetworkType GetType() const { return type_; }
  const NetworkSettings& GetSettings() const { return settings_; }

 private:
  std::vector<double> ExpectedOutput(const Image& image);

  NetworkType type_;
  NetworkSettings settings_;
  std::unique_ptr<NetworkInterface> network_;
};

}  // namespace s21

#endif  // SRC_MODEL_NEURAL_NETWORK_NEURAL_NETWORK_H_
