#pragma once

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <type_traits>

template <int n>
class vec
{
    static_assert(n > 0, "Vector dimension must be positive");
    double v[n] = {};

public:
    vec() = default;

    template <class... Args,
              std::enable_if_t<sizeof...(Args) == n &&
                               (std::is_convertible_v<Args, double> && ...), int> = 0>
    vec(Args... values) : v{static_cast<double>(values)...} {}

    vec(const double (&values)[n])
    {
        for (int i = 0; i < n; i++) v[i] = values[i];
    }

    double& operator[](int i) { assert(i >= 0 && i < n); return v[i]; }
    const double& operator[](int i) const { assert(i >= 0 && i < n); return v[i]; }

    vec& operator+=(const vec& o)
    {
        for (int i = 0; i < n; i++) v[i] += o[i];
        return *this;
    }

    vec& operator-=(const vec& o)
    {
        for (int i = 0; i < n; i++) v[i] -= o[i];
        return *this;
    }

    vec operator+(const vec& o) const { vec result(*this); return result += o; }
    vec operator-(const vec& o) const { vec result(*this); return result -= o; }

    vec operator*(double scalar) const
    {
        vec result;
        for (int i = 0; i < n; i++) result[i] = v[i] * scalar;
        return result;
    }

    friend vec operator*(double scalar, const vec& value) { return value * scalar; }

    vec operator/(double scalar) const
    {
        vec result;
        for (int i = 0; i < n; i++) result[i] = v[i] / scalar;
        return result;
    }

    double dot(const vec& o) const
    {
        double result = 0;
        for (int i = 0; i < n; i++) result += v[i] * o[i];
        return result;
    }

    auto operator^(const vec& o) const
    {
        static_assert(n == 2 || n == 3, "Cross product requires 2D or 3D vectors");
        if constexpr (n == 2) {
            return v[0] * o[1] - v[1] * o[0];
        } else {
            return vec<3>{v[1]*o[2] - v[2]*o[1],
                          v[2]*o[0] - v[0]*o[2],
                          v[0]*o[1] - v[1]*o[0]};
        }
    }

    double norm() const { return std::sqrt(dot(*this)); }
    vec& normalize()
    {
        const double length = norm();
        if (length > 0) *this = *this / length;
        return *this;
    }

    // Dropping trailing components does not perform perspective division.
    template <int m, std::enable_if_t<(m > 0 && m < n), int> = 0>
    operator vec<m>() const
    {
        vec<m> result;
        for (int i = 0; i < m; i++) result[i] = v[i];
        return result;
    }
};

template <int n, int m>
class matrix
{
    static_assert(n > 0 && m > 0, "Matrix dimensions must be positive");
    vec<m> mat[n];

public:
    static constexpr int rows = n;
    static constexpr int cols = m;

    matrix() = default;
    matrix(const double (&a)[n][m])
    {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) mat[i][j] = a[i][j];
        }
    } 
    matrix(const matrix<n, m> &o) 
    {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) mat[i][j] = o[i][j];
        }
    }

    vec<m>& operator[](int i) { assert(i >= 0 && i < n); return mat[i]; }
    const vec<m>& operator[](int i) const { assert(i >= 0 && i < n); return mat[i]; }

    matrix<m, n> transpose() const
    {
        matrix<m, n> result;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++) result[j][i] = mat[i][j];
        return result;
    }

    matrix& operator+=(const matrix& o)
    {
        for (int i = 0; i < n; i++) mat[i] += o[i];
        return *this;
    }

    matrix operator+(const matrix& o) const { matrix result(*this); return result += o; }

    matrix operator/(double scalar) const
    {
        matrix result;
        for (int i = 0; i < n; i++) result[i] = mat[i] / scalar;
        return result;
    }

    template <int k>
    matrix<n, k> matmul(const matrix<m, k>& o) const
    {
        matrix<n, k> result;
        const auto transposed = o.transpose();
        for (int i = 0; i < n; i++)
            for (int j = 0; j < k; j++) result[i][j] = mat[i].dot(transposed[j]);
        return result;
    }

    vec<n> matmul(const vec<m>& o) const
    {
        vec<n> result;
        for (int i = 0; i < n; i++) result[i] = mat[i].dot(o);
        return result;
    }

    double det() const
    {
        static_assert(n == m, "Determinant requires a square matrix");
        if constexpr (n == 1) {
            return mat[0][0];
        } else {
            double result = 0;
            for (int j = 0; j < m; j++) result += mat[0][j] * cofactor(0, j);
            return result;
        }
    }

    double cofactor(int row, int col) const
    {
        static_assert(n == m, "Cofactors require a square matrix");
        if constexpr (n == 1) {
            return 1;
        } else {
            matrix<n-1, m-1> submatrix;
            for (int i = 0; i < n-1; i++)
                for (int j = 0; j < m-1; j++)
                    submatrix[i][j] = mat[i + (i >= row)][j + (j >= col)];
            return submatrix.det() * ((row + col) % 2 ? -1 : 1);
        }
    }

    matrix invert_transpose() const
    {
        static_assert(n == m, "Inverse requires a square matrix");
        const double determinant = det();
        if (determinant == 0) throw std::domain_error("Cannot invert a singular matrix");
        matrix result;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++) result[i][j] = cofactor(i, j);
        return result / determinant;
    }

    matrix invert() const { return invert_transpose().transpose(); }
};

template <int n, int m, int k>
matrix<n, k> operator*(const matrix<n, m>& a, const matrix<m, k>& b)
{
    return a.matmul(b);
}

template <int n, int m>
vec<n> operator*(const matrix<n, m>& a, const vec<m>& b)
{
    return a.matmul(b);
}
