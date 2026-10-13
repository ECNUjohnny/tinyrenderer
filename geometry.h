#include <cmath>

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

    vec3<T> operator + (const vec3<T> &o) const { return vec3<T>(x + o.x, y + o.y, z + o.z); } 
    vec3<T> operator - (const vec3<T> &o) const { return vec3<T>(x - o.x, y -o.y, z - o.z); }
    vec3<T> operator ^ (const vec3<T> &o) const { return vec3<T>(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x); }
    vec3<T> operator * (const float f) const { return vec3<T>(x * f, y * f, z * f); }
    float norm() { return std::sqrt(x*x + y*y + z*z); }
    vec3<T>& normalize() { *this = *this*(1 / norm()); return *this; } 

    T friend dot(const vec3<T> &a, const vec3<T> &b) { return (a.x*b.x+a.y*b.y+a.z*b.z); }

    operator vec2<T> () const { return vec2<T>(x, y); }
};

