#ifndef MATRIX_C
#define MATRIX_C

#include <ostream>
#include <span>
#include <type_traits>
#include <vector>

#include "math.h"

// N number of rows, M number of columns
template <typename T, std::size_t N, std::size_t M> class Matrix {
protected:
  std::vector<T> matData = std::vector<T>(N * M);

public:
  Matrix() = default;
  Matrix(Matrix<T, N, M> &&mat) : matData{std::move(mat.matData)} {};
  Matrix(const Matrix<T, N, M> &mat) = delete;
  Matrix<T, N, M> &operator=(Matrix<T, N, M> mat);
  Matrix<T, N, M> clone() const;

  T &operator()(std::size_t x, std::size_t y);
  const T &operator()(std::size_t x, std::size_t y) const;

  // Basic operations
  // 1) matrix-scalar operations
  Matrix<T, N, M> &operator*=(const T &c1);
  Matrix<T, N, M> &operator/=(const T &c1);

  // 2) matrix-matrix operations
  Matrix<T, N, M> &operator+=(const Matrix<T, N, M> &m1);
  Matrix<T, N, M> &operator-=(const Matrix<T, N, M> &m1);

  // 3) unary operators
  Matrix<T, N, M> operator-() const;
  T norm() const;

  std::span<T, N * M> data() {
    return std::span<T, N * M>(matData.data(), N * M);
  }
};

// Check that i < Max
inline void checkBounds(std::size_t Max, std::size_t i,
                        std::string_view err_msg) {
  if (i >= Max) {
    throw std::runtime_error(err_msg.data());
  }
}

// x column y row
template <typename T, std::size_t N, std::size_t M>
T &Matrix<T, N, M>::operator()(std::size_t x, std::size_t y) {
  checkBounds(N, x, "Matrix<>::operator(): x-coordinate out of bound");
  checkBounds(M, y, "Matrix<>::operator(): y-coordinate out of bound");
  return matData[x + y * N];
}

// x column y row
template <typename T, std::size_t N, std::size_t M>
const T &Matrix<T, N, M>::operator()(std::size_t x, std::size_t y) const {
  checkBounds(N, x, "Matrix<>::operator(): x-coordinate out of bound");
  checkBounds(M, y, "Matrix<>::operator(): y-coordinate out of bound");
  return matData[x + y * N];
}

template <typename T, std::size_t N, std::size_t M>
Matrix<T, N, M> &Matrix<T, N, M>::operator=(Matrix<T, N, M> mat) {
  matData = std::move(mat.matData);
  return *this;
};

template <typename T, std::size_t N, std::size_t M>
Matrix<T, N, M> Matrix<T, N, M>::clone() const {
  Matrix<T, N, M> ret;
  ret.matData = this->matData;
  return ret;
};

template <typename T, std::size_t N, std::size_t M>
std::ostream &operator<<(std::ostream &o, const Matrix<T, N, M> &m1) {
  for (size_t j = 0; j < N; j++) {
    for (size_t i = 0; i < M; i++) {
      o << m1(j, i) << " ";
    }
    o << "\n";
  }
  return o;
}

template <typename T, std::size_t N, std::size_t M>
Matrix<T, N, M> Matrix<T, N, M>::operator-() const {
  Matrix<T, N, M> res = this->clone();
  for (T &el : res.matData) {
    el = -el;
  }
  return res;
}

template <typename T, std::size_t N, std::size_t M>
Matrix<T, N, M> &Matrix<T, N, M>::operator+=(const Matrix<T, N, M> &m1) {
  for (size_t i = 0; i < N * M; i++) {
    matData[i] += m1.matData[i];
  }
  return *this;
}

template <typename T, std::size_t N, std::size_t M>
Matrix<T, N, M> &Matrix<T, N, M>::operator-=(const Matrix<T, N, M> &m1) {
  for (size_t i = 0; i < N * M; i++) {
    matData[i] -= m1.matData[i];
  }
  return *this;
}

template <typename T, std::size_t N, std::size_t M>
Matrix<T, N, M> operator+(const Matrix<T, N, M> &m1,
                          const Matrix<T, N, M> &m2) {
  Matrix<T, N, M> res = m1.clone();
  res += m2;
  return res;
}
template <typename T, std::size_t N, std::size_t M>
Matrix<T, N, M> operator-(const Matrix<T, N, M> &m1,
                          const Matrix<T, N, M> &m2) {
  return m1 + (-m2);
}

template <typename T, std::size_t N, std::size_t M>
T Matrix<T, N, M>::norm() const {
  T norm = 0;
  for (const T &el : matData) {
    norm += el * el;
  }
  return sqrt(norm);
}

template <typename T, std::size_t N, std::size_t M>
Matrix<T, N, M> &Matrix<T, N, M>::operator*=(const T &c1) {
  for (T &el : matData) {
    el *= c1;
  }
  return *this;
}

template <typename T, std::size_t N, std::size_t M>
Matrix<T, N, M> &Matrix<T, N, M>::operator/=(const T &c1) {
  return (*this) *= (1.0 / c1);
}

template <typename T, std::size_t N, std::size_t M>
Matrix<T, N, M> operator*(const Matrix<T, N, M> &m1, const T &c) {
  Matrix<T, N, M> res = m1.clone();
  res *= c;
  return res;
}

template <typename T, std::size_t N, std::size_t M>
Matrix<T, N, M> operator*(const T &c, const Matrix<T, N, M> &m1) {
  return m1 * c;
}

template <typename T, std::size_t N, std::size_t M, std::size_t K>
Matrix<T, N, K> operator*(const Matrix<T, N, M> &m1,
                          const Matrix<T, M, K> &m2) {
  Matrix<T, N, K> res;
  for (size_t j = 0; j < K; j++) {
    for (size_t k = 0; k < M; k++) {
      for (size_t i = 0; i < N; i++) {
        res(i, j) += m1(i, k) * m2(k, j);
      }
    }
  }
  return res;
}

template <typename T, std::size_t N> class Vector : public Matrix<T, N, 1> {
public:
  Vector(Matrix<T, N, 1> &&mat) : Matrix<T, N, 1>{std::move(mat)} {};
  template <typename... U>
    requires(sizeof...(U) == N * 1) && (std::is_same_v<T, U> && ...)
  Vector(U &&...args) {
    this->matData = {std::forward<U>(args)...};
  };
  Vector() = default;
  T &operator()(std::size_t x);
  const T &operator()(std::size_t x) const;

  T &operator()(std::size_t x, std::size_t y) = delete;
  const T &operator()(std::size_t x, std::size_t y) const = delete;
};

template <typename T, std::size_t N>
T &Vector<T, N>::operator()(std::size_t x) {
  return Matrix<T, N, 1>::operator()(x, 0);
}

template <typename T, std::size_t N>
const T &Vector<T, N>::operator()(std::size_t x) const {
  return Matrix<T, N, 1>::operator()(x, 0);
}

// Add here other template specializations
typedef Matrix<float, 4, 4> Mat4f;
typedef Matrix<float, 3, 3> Mat3f;
typedef Vector<float, 3> Vec3f;
typedef Vector<float, 4> Vec4f;

#endif // MATRIX_C