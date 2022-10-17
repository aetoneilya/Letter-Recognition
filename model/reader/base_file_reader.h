#ifndef SRC_MODEL_READER_BASE_FILE_READER_H_
#define SRC_MODEL_READER_BASE_FILE_READER_H_

#include <functional>
#include <list>
#include <string>

#include "../image.h"

namespace s21 {

class BaseFileReader {
 public:
  virtual ~BaseFileReader() = default;

  virtual std::list<Image> Read(const std::string& filename) = 0;
};

}  // namespace s21

#endif  // SRC_MODEL_READER_BASE_FILE_READER_H_
