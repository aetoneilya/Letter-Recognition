#ifndef SRC_MODEL_NEURAL_NETWORK_IO_WEIGHT_READER_H_
#define SRC_MODEL_NEURAL_NETWORK_IO_WEIGHT_READER_H_

#include "../neural_network.h"
#include "weight_writer.h"

namespace s21 {

class WeightReader {
 public:
  struct Data {
    NetworkSettings settings;
    std::size_t epoch;
    std::size_t accuracy;
    std::vector<double> weights;
  };

  WeightReader() = delete;

  static Data Read(const std::string& filename);
};

}  // namespace s21

#endif  // SRC_MODEL_NEURAL_NETWORK_IO_WEIGHT_READER_H_
