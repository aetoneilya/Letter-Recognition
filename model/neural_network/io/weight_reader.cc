#include "weight_reader.h"

namespace s21 {

WeightReader::Data WeightReader::Read(const std::string& filename) {
  Data data;
  std::ifstream file(filename, std::ifstream::binary);
  if (file.is_open()) {
    char signature[sizeof(WeightWriter::kSignature)];
    file.read(reinterpret_cast<char*>(signature), sizeof(signature));

    if (std::string(signature) == WeightWriter::kSignature) {
      file.read(reinterpret_cast<char*>(&data.settings),
                sizeof(NetworkSettings));

      file.read(reinterpret_cast<char*>(&data.epoch), sizeof(data.epoch));
      file.read(reinterpret_cast<char*>(&data.accuracy), sizeof(data.accuracy));

      double d;
      while (file.read(reinterpret_cast<char*>(&d), sizeof(d))) {
        data.weights.push_back(d);
      }
      file.close();
    } else {
      throw std::runtime_error("некорректный формат файла");
    }
  } else {
    throw std::runtime_error("файл не найден");
  }

  return data;
}

}  // namespace s21
