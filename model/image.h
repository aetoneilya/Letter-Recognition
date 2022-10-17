#ifndef SRC_MODEL_IMAGE_H_
#define SRC_MODEL_IMAGE_H_

#include <algorithm>
#include <vector>

namespace s21 {

class Image {
 public:
  static const int kWidthInPx = 28;
  static const int kHeightInPx = 28;
  constexpr static const double kMaxValue = 255.;
  static const int kSizeInPx = kWidthInPx * kHeightInPx;

  Image() : char_number_(0) { data_.reserve(kSizeInPx); }
  explicit Image(const std::vector<double>& data)
      : char_number_(-1), data_(data) {}

  int GetNumber() const { return char_number_; }
  const std::vector<double>& GetData() const { return data_; }

  void NormalizeData() {
    std::transform(data_.begin(), data_.end(), data_.begin(),
                   [](double d) -> double { return d / kMaxValue; });
  }

 private:
  friend class CsvReader;

  int char_number_;
  std::vector<double> data_;
};

}  // namespace s21

#endif  // SRC_MODEL_IMAGE_H_
