#include "s21_matrix_oop.h"

namespace s21 {

Matrix::Matrix(std::size_t rows, std::size_t cols) : rows_(rows), cols_(cols) {
  if (rows == 0 || cols == 0) {
    throw std::invalid_argument("Matrix: rows and columns must be more than 0");
  }

  matrix_ = new double*[rows_];

  for (std::size_t i = 0; i < rows_; i++) {
    matrix_[i] = new double[cols_]();
  }
}

Matrix::Matrix(const std::vector<double>& row) : Matrix(row.size(), 1) {
  for (size_t i = 0; i < row.size(); i++) {
    operator()(i, 0) = row[i];
  }
}

Matrix::Matrix(const Matrix& other) {
  if (other.rows_ > 0 && other.cols_ > 0) {
    Matrix m(other.rows_, other.cols_);
    for (std::size_t i = 0; i < m.rows_; i++) {
      std::memcpy(m.matrix_[i], other.matrix_[i], other.cols_ * sizeof(double));
    }
    Swap(&m);
  }
}

Matrix::Matrix(Matrix&& other) { Swap(&other); }

Matrix::~Matrix() {
  if (matrix_ != nullptr) {
    for (std::size_t i = 0; i < rows_; i++) {
      delete[] matrix_[i];
    }
    delete[] matrix_;
    matrix_ = nullptr;
  }
  rows_ = 0;
  cols_ = 0;
}

void Matrix::Swap(Matrix* other) {
  std::swap(rows_, other->rows_);
  std::swap(cols_, other->cols_);
  std::swap(matrix_, other->matrix_);
}

std::size_t Matrix::GetRows() const { return rows_; }

void Matrix::SetRows(std::size_t rows) {
  if (rows_ != rows) {
    Matrix m(rows, cols_);
    for (std::size_t i = 0; i < std::min(rows_, rows); i++) {
      std::memcpy(m.matrix_[i], matrix_[i], cols_ * sizeof(double));
    }
    Swap(&m);
  }
}

std::size_t Matrix::GetColumns() const { return cols_; }

void Matrix::SetColumns(std::size_t cols) {
  if (cols_ != cols) {
    Matrix m(rows_, cols);
    auto const length = std::min(cols_, cols);
    for (std::size_t i = 0; i < rows_; i++) {
      std::memcpy(m.matrix_[i], matrix_[i], length * sizeof(double));
    }
    Swap(&m);
  }
}

bool Matrix::EqMatrix(const Matrix& other) const {
  bool status = rows_ == other.rows_ && cols_ == other.cols_;
  if (rows_ == 0 || cols_ == 0) {
    throw std::logic_error("EqMatrix: invalid matrix");
  }
  for (std::size_t i = 0; i < rows_ && status; i++) {
    for (std::size_t j = 0; j < cols_ && status; j++) {
      if (std::fabs(matrix_[i][j] - other.matrix_[i][j]) >= kEps) {
        status = false;
      }
    }
  }
  return status;
}

void Matrix::SumMatrix(const Matrix& other) {
  if (rows_ != other.rows_ || cols_ != other.cols_ || rows_ == 0 ||
      cols_ == 0) {
    throw std::logic_error(
        "SumMatrix: invalid matrix or different dimensions of the matrix");
  }
  for (std::size_t i = 0; i < rows_; i++) {
    for (std::size_t j = 0; j < cols_; j++) {
      matrix_[i][j] += other.matrix_[i][j];
    }
  }
}

void Matrix::SubMatrix(const Matrix& other) {
  if (rows_ != other.rows_ || cols_ != other.cols_ || rows_ == 0 ||
      cols_ == 0) {
    throw std::logic_error(
        "SubMatrix: invalid matrix or different dimensions of the matrix");
  }
  for (std::size_t i = 0; i < rows_; i++) {
    for (std::size_t j = 0; j < cols_; j++) {
      matrix_[i][j] -= other.matrix_[i][j];
    }
  }
}

void Matrix::MulNumber(double number) {
  if (rows_ == 0 || cols_ == 0) {
    throw std::logic_error("MulNumber: invalid matrix");
  }
  for (std::size_t i = 0; i < rows_; i++) {
    for (std::size_t j = 0; j < cols_; j++) {
      matrix_[i][j] *= number;
    }
  }
}

void Matrix::MulMatrix(const Matrix& other) {
  if (cols_ != other.rows_ || rows_ == 0 || cols_ == 0 || other.rows_ == 0 ||
      other.cols_ == 0) {
    throw std::logic_error(
        "MulMatrix: invalid matrix or different dimensions of the matrix");
  }
  Matrix m(rows_, other.cols_);
  for (std::size_t i = 0; i < m.rows_; i++) {
    for (std::size_t j = 0; j < m.cols_; j++) {
      for (std::size_t k = 0; k < cols_; k++) {
        m.matrix_[i][j] += matrix_[i][k] * other.matrix_[k][j];
      }
    }
  }
  Swap(&m);
}

Matrix Matrix::Transpose() const {
  if (rows_ == 0 || cols_ == 0) {
    throw std::logic_error("Transpose: invalid matrix");
  }
  Matrix m(cols_, rows_);
  for (std::size_t i = 0; i < m.rows_; i++) {
    for (std::size_t j = 0; j < m.cols_; j++) {
      m.matrix_[i][j] = matrix_[j][i];
    }
  }
  return m;
}

Matrix Matrix::CalcComplements() const {
  if (rows_ != cols_ || rows_ == 0) {
    throw std::logic_error(
        "CalcComplements: invalid matrix or matrix is not square");
  }
  Matrix m(cols_, rows_);
  if (rows_ > 1) {
    Matrix t(rows_ - 1, cols_ - 1);
    for (std::size_t i = 0; i < m.rows_; i++) {
      for (std::size_t j = 0; j < m.cols_; j++) {
        unsigned int k_ = 0;
        for (std::size_t k = 0; k < m.rows_; k++) {
          unsigned int l_ = 0;
          for (std::size_t l = 0; l < m.cols_; l++) {
            if (k == i) k_ = 1;
            if (l == j) l_ = 1;
            if (k != i && l != j) t.matrix_[k - k_][l - l_] = matrix_[k][l];
          }
        }
        m.matrix_[i][j] = t.Determinant();
        if ((i + j) % 2 != 0) {
          m.matrix_[i][j] = -m.matrix_[i][j];
        }
      }
    }

  } else {
    m.matrix_[0][0] = matrix_[0][0];
  }
  return m;
}

double Matrix::Determinant() const {
  if (rows_ != cols_ || rows_ == 0) {
    throw std::logic_error(
        "Determinant: invalid matrix or matrix is not square");
  }
  double determinant = 1;

  Matrix m(*this);
  for (std::size_t i = 0; i < m.cols_ && determinant != 0; i++) {
    if (m.DeterminantSwapRow(i)) {
      determinant = -determinant;
    }
    if (m.matrix_[i][i] == 0) {
      determinant = 0;
    }
    for (auto j = i + 1; j < m.rows_ && determinant != 0; j++) {
      double factor = -m.matrix_[j][i] / m.matrix_[i][i];
      for (std::size_t k = 0; k < m.cols_; k++) {
        m.matrix_[j][k] += m.matrix_[i][k] * factor;
      }
    }
  }

  for (std::size_t i = 0; i < m.cols_ && determinant != 0; i++) {
    determinant *= m.matrix_[i][i];
  }

  return determinant;
}

bool Matrix::DeterminantSwapRow(std::size_t j) {
  std::size_t index = 0;
  double max = 0;
  for (auto i = j; i < rows_; i++) {
    if (std::fabs(matrix_[i][j]) > max) {
      index = i;
      max = std::fabs(matrix_[i][j]);
    }
  }

  bool need_swap = j != index;
  if (need_swap) {
    std::swap(matrix_[j], matrix_[index]);
  }
  return need_swap;
}

Matrix Matrix::InverseMatrix() const {
  double d = Determinant();
  if (d == 0) {
    throw std::runtime_error("InverseMatrix: matrix determinant is 0");
  }
  Matrix complements = CalcComplements();
  Matrix m = complements.Transpose();
  for (std::size_t i = 0; i < m.rows_; i++) {
    for (std::size_t j = 0; j < m.cols_; j++) {
      m.matrix_[i][j] /= d;
    }
  }
  return m;
}

Matrix Matrix::operator+(const Matrix& other) const {
  Matrix m(*this);
  m.SumMatrix(other);
  return m;
}

Matrix Matrix::operator-(const Matrix& other) const {
  Matrix m(*this);
  m.SubMatrix(other);
  return m;
}

Matrix Matrix::operator*(const Matrix& other) const {
  Matrix m(*this);
  m.MulMatrix(other);
  return m;
}

Matrix operator*(Matrix const& self, double number) {
  Matrix m(self);
  m.MulNumber(number);
  return m;
}

Matrix operator*(double number, Matrix const& self) {
  return operator*(self, number);
}

bool Matrix::operator==(const Matrix& other) const { return EqMatrix(other); }

Matrix& Matrix::operator=(const Matrix& other) {
  if (this != &other) {
    Matrix(other).Swap(this);
  }
  return *this;
}

Matrix& Matrix::operator=(Matrix&& other) {
  if (this != &other) {
    Matrix(std::move(other)).Swap(this);
  }
  return *this;
}

Matrix& Matrix::operator+=(const Matrix& other) {
  SumMatrix(other);
  return *this;
}

Matrix& Matrix::operator-=(const Matrix& other) {
  SubMatrix(other);
  return *this;
}

Matrix& Matrix::operator*=(const Matrix& other) {
  MulMatrix(other);
  return *this;
}

Matrix& Matrix::operator*=(double const number) {
  MulNumber(number);
  return *this;
}

double& Matrix::operator()(std::size_t i, std::size_t j) {
  return const_cast<double&>(const_cast<const Matrix*>(this)->operator()(i, j));
}

double const& Matrix::operator()(std::size_t i, std::size_t j) const {
  if (i >= rows_ || j >= cols_) {
    throw std::out_of_range("operator(): i or j is out of range");
  }
  return matrix_[i][j];
}

}  // namespace s21
