#ifndef __GFX_MATH_H__
#define __GFX_MATH_H__

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <assert.h>
#include <algorithm>

#ifdef WIN32
#undef min
#undef max
#endif

namespace GfxMath {

inline void gfxAssert(bool expr, const char* error) {
  if (!expr) {
    //std::cout << error << std::endl;
  }
  assert(expr);
}

template <typename T, int N> class Vector {
public:
  Vector() {
  }

  explicit Vector(const T& rhs) {
    for (int i = 0; i < N; ++i) {
      m[i] = rhs;
    }
  }

  explicit Vector(const T& a, const T& b) {
    gfxAssert(N == 2, "Error: vector cannot be constructed from 2 scalars.");
    m[0] = a;
    m[1] = b;
  }
  
  explicit Vector(const T& a, const T& b, const T& c) {
    gfxAssert(N == 3, "Error: vector cannot be constructed from 3 scalars.");
    m[0] = a;
    m[1] = b;
    m[2] = c;
  }
  
  explicit Vector(const T& a, const T& b, const T& c, const T& d) {
    gfxAssert(N == 4, "Error: vector cannot be constructed from 4 scalars.");
    m[0] = a;
    m[1] = b;
    m[2] = c;
    m[3] = d;
  }

  explicit Vector(const Vector<T,2>& a, const T& b) {
    gfxAssert(N == 3, "Error: vector cannot be constructed from a 2-element vector and a scalar.");
    m[0] = a[0];
    m[1] = a[1];
    m[2] = b;
  }
  
  explicit Vector(const Vector<T,3>& a, const T& b) {
    gfxAssert(N == 4, "Error: vector cannot be constructed from a 3-element vector and a scalar.");
    m[0] = a[0];
    m[1] = a[1];
    m[2] = a[2];
    m[3] = b;
  }
  
  explicit Vector(const Vector<T,2>& a, const Vector<T,2>& b) {
    gfxAssert(N == 4, "Error: vector cannot be constructed from two 2-element vectors.");
    m[0] = a[0];
    m[1] = a[1];
    m[2] = b[0];
    m[3] = b[1];
  }

  Vector(const Vector<T,N>& rhs) {
    *this = rhs;
  }
  
  Vector<T,N>& operator =(const Vector<T,N>& rhs) {
    for (int i = 0; i < N; ++i) {
      m[i] = rhs[i];
    }
    return *this;
  }

  T& operator [](const int i) {
    gfxAssert(i < N, "Error: vector array index out of bounds.");
    return m[i];
  }
  
  const T& operator [](const int i) const {
    gfxAssert(i < N, "Error: vector array index out of bounds.");
    return m[i];
  }
  
  Vector<T,N> operator -() const {
    Vector<T,N> r;
    for (int i = 0; i < N; ++i) {
      r[i] = -m[i];
    }
    return r;
  }
  
  Vector<T,N> operator *(const T& rhs) const {
    Vector<T,N> r;
    for (int i = 0; i < N; ++i) {
      r[i] = m[i] * rhs;
    }
    return r;
  }
  
  Vector<T,N> operator *(const Vector<T,N>& rhs) const {
    Vector<T,N> r;
    for (int i = 0; i < N; ++i) {
      r[i] = m[i] * rhs[i];
    }
    return r;
  }
  
  Vector<T,N>& operator *=(const T& rhs) {
    *this = *this * rhs;
  }
  
  Vector<T,N>& operator *=(const Vector<T,N>& rhs) {
    *this = *this * rhs;
  }
  
  Vector<T,N> operator /(const T& rhs) const {
    Vector<T,N> r;
    for (int i = 0; i < N; ++i) {
      r[i] = m[i] / rhs;
    }
    return r;
  }
  
  Vector<T,N>& operator /=(const T& rhs) {
    *this = *this / rhs;
  }
  
  Vector<T,N>& operator /=(const Vector<T,N>& rhs) {
    *this = *this / rhs;
  }
  
  Vector<T,N> operator %(const T& rhs) const {
    Vector<T,N> r;
    for (int i = 0; i < N; ++i) {
      r[i] = m[i] % rhs;
    }
    return r;
  }
  
  Vector<T,N> operator %(const Vector<T,N>& rhs) const {
    Vector<T,N> r;
    for (int i = 0; i < N; ++i) {
      r[i] = m[i] % rhs[i];
    }
    return r;
  }
  
  Vector<T,N>& operator %=(const T& rhs) {
    *this = *this % rhs;
  }
  
  Vector<T,N>& operator %=(const Vector<T,N>& rhs) {
    *this = *this % rhs;
  }
  
  Vector<T,N> operator +(const T& rhs) const {
    Vector<T,N> r;
    for (int i = 0; i < N; ++i) {
      r[i] = m[i] + rhs;
    }
    return r;
  }
  
  Vector<T,N> operator +(const Vector<T,N>& rhs) const {
    Vector<T,N> r;
    for (int i = 0; i < N; ++i) {
      r[i] = m[i] + rhs[i];
    }
    return r;
  }
  
  Vector<T,N>& operator +=(const T& rhs) {
    *this = *this + rhs;
  }
  
  Vector<T,N>& operator +=(const Vector<T,N>& rhs) {
    *this = *this + rhs;
  }

  Vector<T,N> operator -(const T& rhs) const {
    Vector<T,N> r;
    for (int i = 0; i < N; ++i) {
      r[i] = m[i] - rhs;
    }
    return r;
  }
  
  Vector<T,N> operator -(const Vector<T,N>& rhs) const {
    Vector<T,N> r;
    for (int i = 0; i < N; ++i) {
      r[i] = m[i] - rhs[i];
    }
    return r;
  }
  
  Vector<T,N>& operator -=(const T& rhs) {
    *this = *this - rhs;
  }
  
  Vector<T,N>& operator -=(const Vector<T,N>& rhs) {
    *this = *this - rhs;
  }
  
  T& permute(const int x) {
    gfxAssert(x < N, "Error: vector permute index out of bounds.");
    return m[x];
  }
  
  const T& permute(const int x) const {
    gfxAssert(x < N, "Error: vector permute index out of bounds.");
    return m[x];
  }
  
  const Vector<T,2> permute(const int x, const int y) const {
    gfxAssert(x < N && y < N, "Error: vector permute index out of bounds.");
    Vector<T,2> r;
    r[0] = m[x];
    r[1] = m[y];
    return r;
  }
  
  const Vector<T,3> permute(const int x, const int y, const int z) const {
    gfxAssert(x < N && y < N && z < N, "Error: vector permute index out of bounds.");
    Vector<T,3> r;
    r[0] = m[x];
    r[1] = m[y];
    r[2] = m[z];
    return r;
  }
  
  const Vector<T,3> permute(const int x, const int y, const int z, const int w) const {
    gfxAssert(x < N && y < N && z < N && w < N, "Error: vector permute index out of bounds.");
    Vector<T,4> r;
    r[0] = m[x];
    r[1] = m[y];
    r[2] = m[z];
    r[3] = m[w];
    return r;
  }

private:
  T m[N];
};

/* vector operators */

template <typename T, int N>
Vector<T,N> operator *(const T& lhs, const Vector<T,N>& rhs) {
    Vector<T,N> r;
    for (int i = 0; i < N; ++i) {
      r[i] = lhs * rhs[i];
    }
    return r;
}

template <typename T, int N>
Vector<T,N> operator /(const T& lhs, const Vector<T,N>& rhs) {
    Vector<T,N> r;
    for (int i = 0; i < N; ++i) {
      r[i] = lhs / rhs[i];
    }
    return r;
}

template <typename T, int N>
Vector<T,N> operator %(const T& lhs, const Vector<T,N>& rhs) {
    Vector<T,N> r;
    for (int i = 0; i < N; ++i) {
      r[i] = lhs % rhs[i];
    }
    return r;
}

template <typename T, int N>
Vector<T,N> operator +(const T& lhs, const Vector<T,N>& rhs) {
    Vector<T,N> r;
    for (int i = 0; i < N; ++i) {
      r[i] = lhs + rhs[i];
    }
    return r;
}

template <typename T, int N>
Vector<T,N> operator -(const T& lhs, const Vector<T,N>& rhs) {
    Vector<T,N> r;
    for (int i = 0; i < N; ++i) {
      r[i] = lhs - rhs[i];
    }
    return r;
}

/* M = columns, N = rows, column major */
template <typename T, int M, int N> class Matrix {
public:
  Matrix() {
  }

  Matrix(const T& a0, const T& a1, const T& a2, const T& a3) {
    gfxAssert(M == 2 && N == 2, "Error: wrong number of scalar elements to matrix constructor.");
    m[0][0] = a0;
    m[0][1] = a1;
    m[1][0] = a2;
    m[1][1] = a3;
  }
  
  Matrix(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5, const T& a6, const T& a7, const T& a8) {
    gfxAssert(M == 3 && N == 3, "Error: wrong number of scalar elements to matrix constructor.");
    m[0][0] = a0;
    m[0][1] = a1;
    m[0][2] = a2;
    m[1][0] = a3;
    m[1][1] = a4;
    m[1][2] = a5;
    m[2][0] = a6;
    m[2][1] = a7;
    m[2][2] = a8;
  }
  
  Matrix(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5, const T& a6, const T& a7, const T& a8, const T& a9, const T& a10, const T& a11, const T& a12, const T& a13, const T& a14, const T& a15) {
    gfxAssert(M == 4 && N == 4, "Error: wrong number of scalar elements to matrix constructor.");
    m[0][0] = a0;
    m[0][1] = a1;
    m[0][2] = a2;
    m[0][3] = a3;
    m[1][0] = a4;
    m[1][1] = a5;
    m[1][2] = a6;
    m[1][3] = a7;
    m[2][0] = a8;
    m[2][1] = a9;
    m[2][2] = a10;
    m[2][3] = a11;
    m[2][0] = a12;
    m[2][1] = a13;
    m[2][2] = a14;
    m[2][3] = a15;
  }

  Matrix(const Vector<T,2>& a, const Vector<T,2>& b) {
    gfxAssert(M == 2 && N == 2, "Error: wrong number of vector elements to 2x2 matrix constructor.");
    m[0] = a;
    m[1] = b;
  }
  
  Matrix(const Vector<T,3>& a, const Vector<T,3>& b, const Vector<T,3>& c) {
    gfxAssert(M == 3 && N == 3, "Error: wrong number of vector elements to 3x3 matrix constructor.");
    m[0] = a;
    m[1] = b;
    m[2] = c;
  }
  
  Matrix(const Vector<T,4>& a, const Vector<T,4>& b, const Vector<T,4>& c, const Vector<T,4>& d) {
    gfxAssert(M == 4 && N == 4, "Error: wrong number of vector elements to 4x4 matrix constructor.");
    m[0] = a;
    m[1] = b;
    m[2] = c;
    m[3] = d;
  }

  Matrix(const Matrix<T,M,N>& rhs) {
    *this = rhs;
  }
  
  Matrix<T,M,N>& operator =(const Matrix<T,M,N>& rhs) {
    for (int i = 0; i < M; ++i) {
      for (int j = 0; j < N; ++j) {
        m[i][j] = rhs[i][j];
      }
    }
    return *this;
  }

  Vector<T, M>& operator [](const int i) {
    gfxAssert(i < M, "Error: matrix array index out of bounds.");
    return m[i];
  }
  
  const Vector<T, M>& operator [](const int i) const {
    gfxAssert(i < M, "Error: matrix array index out of bounds.");
    return m[i];
  }
  
private:
  Vector<T, N> m[M];
};

/* scalar functions */

inline double min(const double a, const double b) {
  return std::min(a, b);
}

inline double max(const double a, const double b) {
  return std::max(a, b);
}

inline double clamp(const double x, const double a, const double b) {
  return max(min(x, b), a);
}

inline double abs(const double x) {
  return fabs(x);
}

inline double sign(const double x) {
  return (x == 0 ? 0 : (x < 0 ? -1 : 1));
}

inline double trunc(const double x) {
  return double(int(x));
}

inline double fract(const double x) {
  return x - floor(x);
}

inline double mod(const double x, const double y) {
  return x - y * floor(x/y);
}

inline double mix(const double x, const double y, const double a) {
  return x*(1-a) + y*a;
}

/* vector functions */

template <typename T, int N>
Vector<T,N> min(const Vector<T,N>& a, const T& b) {
  return min(a, Vector<T,N>(b));
}

template <typename T, int N>
Vector<T,N> min(const T& b, const Vector<T,N>& a) {
  return min(Vector<T,N>(a), b);
}

template <typename T, int N>
Vector<T,N> min(const Vector<T,N>& a, const Vector<T,N>& b) {
  Vector<T,N> r;
  for (int i = 0; i < N; ++i) {
    r[i] = min(a[i], b[i]);
  }
  return r;
}

template <typename T, int N>
Vector<T,N> max(const Vector<T,N>& a, const T& b) {
  return max(a, Vector<T,N>(b));
}

template <typename T, int N>
Vector<T,N> max(const T& b, const Vector<T,N>& a) {
  return max(Vector<T,N>(a), b);
}

template <typename T, int N>
Vector<T,N> max(const Vector<T,N>& a, const Vector<T,N>& b) {
  Vector<T,N> r;
  for (int i = 0; i < N; ++i) {
    r[i] = max(a[i], b[i]);
  }
  return r;
}

template <typename T, int N>
Vector<T,N> abs(const Vector<T,N>& v) {
  Vector<T,N> r;
  for (int i = 0; i < N; ++i) {
    r[i] = T(abs(v[i]));
  }
  return r;
}

template <typename T, int N>
Vector<T,N> sign(const Vector<T,N>& v) {
  Vector<T,N> r;
  for (int i = 0; i < N; ++i) {
    r[i] = T(sign(v[i]));
  }
  return r;
}

template <typename T, int N>
Vector<T,N> floor(const Vector<T,N>& v) {
  Vector<T,N> r;
  for (int i = 0; i < N; ++i) {
    r[i] = T(floor(v[i]));
  }
  return r;
}

template <typename T, int N>
Vector<T,N> trunc(const Vector<T,N>& v) {
  Vector<T,N> r;
  for (int i = 0; i < N; ++i) {
    r[i] = T(int(v[i]));
  }
  return r;
}

template <typename T, int N>
Vector<T,N> ceil(const Vector<T,N>& v) {
  Vector<T,N> r;
  for (int i = 0; i < N; ++i) {
    r[i] = T(ceil(v[i]));
  }
  return r;
}

template <typename T, int N>
Vector<T,N> fract(const Vector<T,N>& v) {
  Vector<T,N> r;
  for (int i = 0; i < N; ++i) {
    r[i] = T(fract(v[i]));
  }
  return r;
}

template <typename T, int N>
Vector<T,N> mod(const Vector<T,N>& a, const Vector<T,N>& b) {
  Vector<T,N> r;
  for (int i = 0; i < N; ++i) {
    r[i] = T(mod(a[i], b[i]));
  }
  return r;
}

template <typename T, int N>
Vector<T,N> mod(const Vector<T,N>& a, const T& b) {
  return mod(a, Vector<T,N>(b));
}

template <typename T, int N>
Vector<T,N> mod(const T& a, const Vector<T,N>& b) {
  return mod(Vector<T,N>(a), b);
}

template <typename T, int N>
Vector<T,N> mix(const Vector<T,N>& x, const Vector<T,N>& y, const Vector<T,N>& a) {
  Vector<T,N> r;
  for (int i = 0; i < N; ++i) {
    r[i] = T(mix(x[i], y[i], a[i]));
  }
  return r;
}

template <typename T, int N>
Vector<T,N> mix(const Vector<T,N>& x, const Vector<T,N>& y, const T& a) {
  return mix(x, y, Vector<T,N>(a));
}

template <typename T>
Vector<T,3> cross(const Vector<T,3>& lhs, const Vector<T,3>& rhs) {
  Vector<T,3> r;
  r[0] = lhs[1]*rhs[2] - rhs[1]*lhs[2];
  r[1] = lhs[2]*rhs[0] - rhs[2]*lhs[0];
  r[2] = lhs[0]*rhs[1] - rhs[0]*lhs[1];  
  return r;
}

template <typename T, int N>
T dot(const Vector<T,N>& lhs, const Vector<T,N>& rhs) {
  T r = T(0);
  for (int i = 0; i < N; ++i) {
    r += lhs[i] * rhs[i];
  }
  return r;
}

template <typename T, int N>
T length(const Vector<T,N>& v) {
  return T(sqrt(dot(v,v)));
}

template <typename T, int N>
T distance(const Vector<T,N>& lhs, const Vector<T,N>& rhs) {
  return length(lhs - rhs);
}

template <typename T, int N>
Vector<T,N> normalize(const Vector<T,N>& v) {
  return v / length(v);
}

template <typename T, int N>
Vector<T,N> faceforward(const Vector<T,N>& a, const Vector<T,N>& b, const Vector<T,N>& c) {
  if (dot(c,b) < 0) {
    return a;
  } else {
    return -a;
  }
}

template <typename T, int N>
Vector<T,N> reflect(const Vector<T,N>& v, const Vector<T,N>& n) {
  return v - 2 * dot(n,v) * n;
}

template <typename T, int N>
Vector<T,N> refract(const Vector<T,N>& v, const Vector<T,N>& n, const T& eta) {
  T u = dot(n,v);
  T k = 1 - eta * eta * (1 - u * u);
  if (k < 0) {
    return Vector<T,N>(0);
  } else {
    return eta * v - (eta * u + T(sqrt(k))) * n;
  }
}

/* matrix functions */

template <typename T, int M, int N>
Matrix<T,M,N> matrixCompMult(const Matrix<T,M,N>& lhs, const Matrix<T,M,N>& rhs) {
  Matrix<T,M,N> r;
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < M; ++j) {
      r[i][j] = lhs[i][j] * rhs[i][j];
    }
  }
  return r;
}

template <typename T, int M, int N>
Matrix<T,N,M> outerProduct(const Vector<T,M>& lhs, const Vector<T,N>& rhs) {
  Matrix<T,N,M> r;
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < M; ++j) {
      r[i][j] = lhs[j] * rhs[i];
    }
  }
  return r;
}

template <typename T, int M, int N, int P>
Matrix<T,M,P> mul(const Matrix<T,M,N>& lhs, const Matrix<T,N,P>& rhs) {
  Matrix<T,M,P> r;
  for (int i = 0; i < M; ++i) {
    for (int j = 0; j < P; ++j) {
      T s = 0;
      for (int k = 0; k < N; ++k) {
        s += lhs[i][k] * rhs[k][j];
      }
      r[i][j] = s;
    }
  }
  return r;
}

template <typename T, int M, int N, int P>
Matrix<T,M,P> operator *(const Matrix<T,M,N>& lhs, const Matrix<T,N,P>& rhs) {
  return mul(lhs, rhs);
}

template <typename T, int M, int N>
Vector<T,M> mul(const Vector<T,N>& lhs, const Matrix<T,M,N>& rhs) {
  Vector<T,M> r;
  for (int i = 0; i < M; ++i) {
    r[i] = dot(lhs, rhs[i]);
  }
  return r;
}

template <typename T, int M, int N>
Vector<T,M> operator *(const Vector<T,N>& lhs, const Matrix<T,M,N>& rhs) {
  return mul(lhs, rhs);
}

template <typename T, int M, int N>
Vector<T,N> mul(const Matrix<T,M,N>& lhs, const Vector<T,M>& rhs) {
  Vector<T,N> r;
  for (int i = 0; i < N; ++i) {
    T s = 0.0;
    for (int j = 0; j < M; ++j) {
      s += lhs[j][i] * rhs[j];
    }
    r[i] = s;
  }
  return r;
}

template <typename T, int M, int N>
Vector<T,N> operator *(const Matrix<T,M,N>& lhs, const Vector<T,M>& rhs) {
  return mul(lhs, rhs);
}

template <typename T, int M, int N>
Matrix<T,N,M> transpose(const Matrix<T,M,N>& m) {
  Matrix<T,N,M> r;
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < M; ++j) {
      r[i][j] = m[j][i];
    }
  }
  return r;
}

/* types */

typedef Vector<float,2> vec2;
typedef Vector<float,3> vec3;
typedef Vector<float,3> vec4;
typedef Matrix<float,2,2> mat2;
typedef Matrix<float,3,3> mat3;
typedef Matrix<float,4,4> mat4;
typedef Matrix<float,2,2> mat2x2;
typedef Matrix<float,3,3> mat3x3;
typedef Matrix<float,4,4> mat4x4;

} // namespace

#endif // __GFX_MATH_H__
