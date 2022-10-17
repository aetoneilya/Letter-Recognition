#ifndef SRC_MODEL_NEURAL_NETWORK_IO_WEIGHT_WRITER_H_
#define SRC_MODEL_NEURAL_NETWORK_IO_WEIGHT_WRITER_H_

#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "../network_interface.h"

namespace s21 {

class WeightWriter {
 public:
  static void Write(const std::string& filename,
                    const std::vector<double>& weights,
                    const NetworkSettings& settings,
                    std::size_t epoch = std::string::npos,
                    std::size_t accuracy = std::string::npos);

  static void Write(const std::vector<double>& weights,
                    NetworkSettings settings,
                    std::size_t epoch = std::string::npos,
                    std::size_t accuracy = std::string::npos);

 private:
  constexpr static const char kSignature[] = {"SCHOOL21"};

  friend class WeightReader;

  static std::string GenerateFilename(std::size_t layers, std::size_t epoch,
                                      std::size_t accuracy);

  static void Write_(const std::string& filename,
                     const std::vector<double>& weights,
                     const NetworkSettings& settings, std::size_t epoch,
                     std::size_t accuracy);
};

}  // namespace s21

#endif  // SRC_MODEL_NEURAL_NETWORK_IO_WEIGHT_WRITER_H_
