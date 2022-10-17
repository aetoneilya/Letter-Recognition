#ifndef SRC_MODEL_READER_CSV_READER_H_
#define SRC_MODEL_READER_CSV_READER_H_

#ifdef DEBUG
#include <chrono>  // NOLINT [build/c++11]
#include <iostream>
#endif

#include <clocale>
#include <fstream>
#include <stdexcept>
#include <thread>  // NOLINT [build/c++11]
#include <type_traits>

#include "base_file_reader.h"

namespace s21 {

class CsvReader : public BaseFileReader {
 public:
  CsvReader() = default;
  ~CsvReader() = default;

  std::list<Image> Read(const std::string& filename) override;

 private:
  int ReadInt(const std::string& data, std::size_t start_pos,
              std::size_t* end_pos);
  double ReadDouble(const std::string& data, std::size_t start_pos,
                    std::size_t* end_pos);
};

}  // namespace s21

#endif  // SRC_MODEL_READER_CSV_READER_H_
