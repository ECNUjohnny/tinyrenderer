#include <cmath>
#include <vector>
#include <cstring>

template <class T>
class vec2
{

public: 
    T x, y;

    vec2(): x{0}, y{0} {}
    vec2(T x, T y): x{x}, y{y} {}

    vec2<T>& operator +=(const vec2<T> &o)
    {
        x += o.x, y += o.y;
        return *this;
    }

    vec2<T>& operator -=(const vec2<T> &o)
    {
        x -= o.x, y -= o.y;
        return *this;
    }

    vec2<T> operator +(const vec2<T> &o) const 
    {
        vec2<T> tmp(*this);
        return tmp += o;
    }
    
    vec2<T> operator -(const vec2<T> &o) const
    {
        vec2<T> tmp(*this);
        return tmp -= o;
    }

    vec2<T> operator *(const int a) const { return vec2<T>(x * a, y * a); } 
    T operator ^ (const vec2<T> &o) { return x*o.y-y*o.x; }
};

template <class T>
class vec3
{
public:

    T x, y, z;

    vec3(): x(0), y(0), z(0) {}
    vec3(T x, T y, T z): x(x), y(y), z(z) {}
    vec3(const T (&a)[3]): x(a[0]), y(a[1]), z(a[2]) {}

    vec3<T>& operator += (const vec3<T> &o) { x += o.x, y += o.y, z += o.z; return *this; } 
    vec3<T> operator + (const vec3<T> &o) const { return vec3<T>(x + o.x, y + o.y, z + o.z); } 
    vec3<T> operator - (const vec3<T> &o) const { return vec3<T>(x - o.x, y -o.y, z - o.z); }
    vec3<T> operator ^ (const vec3<T> &o) const { return vec3<T>(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x); }
    vec3<T> operator * (const float f) const { return vec3<T>(x * f, y * f, z * f); }
    vec3<T> operator / (const float f) { return vec3<T>(x / f, y / f, z / f); }
    float norm() { return std::sqrt(x*x + y*y + z*z); }
    vec3<T>& normalize() { *this = *this*(1 / norm()); return *this; } 

    T dot(const vec3<T> &v) { return (x*v.x+y*v.y+z*v.z); }

    operator vec2<T> () const { return vec2<T>(x, y); }
};

template <class t>
class matrix3x3
{
public:
    std::vector<vec3<t>> mat = std::vector<vec3<t>>(3);

    matrix3x3() {}
    matrix3x3(const matrix3x3<t> &o)
    {
        for (int i = 0; i < 3; i++) mat[i] = o.mat[i];
    }
    matrix3x3(const t (&a)[3][3])
    {
        for (int i = 0; i < 3; i++)
        {
            vec3<t> v(a[i][0], a[i][1], a[i][2]);
            mat[i] = v;
        }
    }

    matrix3x3<t>& operator += (const matrix3x3<t> &o)
    {
        for (int i = 0; i < 3; i++) mat[i] += o.mat[i]; 
        return *this;    
    }
    
    matrix3x3<t> operator + (const matrix3x3<t> &o)
    {
        matrix3x3<t> ret;
        for (int i = 0; i < 3; i++) ret.mat[i] = o.mat[i] + mat[i]; 
        return ret;   
    }

    vec3<t> dot(vec3<t> v) { return vec3<t>(mat[0].dot(v), mat[1].dot(v), mat[2].dot(v)); }
};


