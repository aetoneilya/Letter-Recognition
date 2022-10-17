#include "csv_reader.h"

namespace s21 {

std::list<Image> CsvReader::Read(const std::string& filename) {
#ifdef DEBUG
  auto start_time = std::chrono::high_resolution_clock::now();
#endif

  std::list<Image> images;

  std::ifstream file(filename);
  if (file.is_open()) {
    std::string data;
    while (std::getline(file, data)) {
      Image image;

      std::size_t i = 0;
      image.char_number_ = ReadInt(data, i, &i);

      while (i < data.length()) {
        image.data_.push_back(ReadDouble(data, i, &i));
      }
      images.push_back(image);
    }
    file.close();
  } else {
    throw std::runtime_error("файл не найден");
  }

#ifdef DEBUG
  auto end_time = std::chrono::high_resolution_clock::now();
  std::cout << "s21::CsvReader[" << this
            << "]::Read(const std::string& filename): "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
                                                                     start_time)
                   .count()
            << " ms." << std::endl;
#endif

  return images;
}

int CsvReader::ReadInt(const std::string& data, std::size_t start_pos,
                       std::size_t* end_pos) {
  std::size_t bias = 0;
  int value;
  try {
    value = std::stoi(data.substr(start_pos), &bias);
  } catch (const std::invalid_argument& e) {
    throw std::runtime_error("некорректный формат данных");
  }
  *end_pos = start_pos + bias + 1;
  return value;
}

double CsvReader::ReadDouble(const std::string& data, std::size_t start_pos,
                             std::size_t* end_pos) {
  std::size_t bias = 0;
  double value;
  try {
    value = std::stod(data.substr(start_pos), &bias);
  } catch (const std::invalid_argument& e) {
    throw std::runtime_error("некорректный формат данных");
  }
  *end_pos = start_pos + bias + 1;
  return value;
}

}  // namespace s21
