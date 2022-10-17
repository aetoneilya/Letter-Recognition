#include "weight_writer.h"

namespace s21 {

void WeightWriter::Write(const std::string& filename,
                         const std::vector<double>& weights,
                         const NetworkSettings& settings, std::size_t epoch,
                         std::size_t accuracy) {
  Write_(filename, weights, settings, epoch, accuracy);
}

void WeightWriter::Write(const std::vector<double>& weights,
                         NetworkSettings settings, std::size_t epoch,
                         std::size_t accuracy) {
  Write_(GenerateFilename(settings.number_of_hidden_layers, epoch, accuracy),
         weights, settings, epoch, accuracy);
}

std::string WeightWriter::GenerateFilename(std::size_t layers,
                                           std::size_t epoch,
                                           std::size_t accuracy) {
  std::stringstream ss;

  ss << "mlp";

  if (layers != std::string::npos) ss << "_l" << layers;
  if (epoch != std::string::npos) ss << "_e" << epoch;
  if (accuracy != std::string::npos) ss << "_a" << accuracy;

  auto time = std::time(nullptr);
  ss << "_" << std::put_time(std::localtime(&time), "%Y-%m-%d_%H-%M-%S");

  ss << ".bin";

  return ss.str();
}

void WeightWriter::Write_(const std::string& filename,
                          const std::vector<double>& weights,
                          const NetworkSettings& settings, std::size_t epoch,
                          std::size_t accuracy) {
  std::ofstream file(filename, std::ofstream::app);
  if (file.is_open()) {
    file.clear();

    file.write(kSignature, sizeof(kSignature));

    file.write(reinterpret_cast<const char*>(&settings),
               sizeof(NetworkSettings));

    file.write(reinterpret_cast<const char*>(&epoch), sizeof(epoch));
    file.write(reinterpret_cast<const char*>(&accuracy), sizeof(accuracy));

    for (auto w : weights) {
      file.write(reinterpret_cast<char*>(&w), sizeof(w));
    }

    file.close();
  }
}

}  // namespace s21
