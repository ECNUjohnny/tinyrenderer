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


template <class T>
class vec4
{
public:

    T x, y, z, w;

    vec4(): x(0), y(0), z(0), w(0) {}
    vec4(T x, T y, T z, T w): x(x), y(y), z(z), w(w) {}
    vec4(const T (&a)[4]): x(a[0]), y(a[1]), z(a[2]), w(a[3]) {}

    vec4<T>& operator += (const vec4<T> &o) { x += o.x, y += o.y, z += o.z, w += o.w; return *this; }
    vec4<T> operator + (const vec4<T> &o) const { return vec4<T>(x + o.x, y + o.y, z + o.z, w + o.w); }
    vec4<T> operator - (const vec4<T> &o) const { return vec4<T>(x - o.x, y - o.y, z - o.z, w - o.w); }
    vec4<T> operator * (const float f) const { return vec4<T>(x * f, y * f, z * f, w * f); }
    vec4<T> operator / (const float f) const { return vec4<T>(x / f, y / f, z / f, w / f); }
    float norm() const { return std::sqrt(x*x + y*y + z*z + w*w); }
    vec4<T>& normalize() { *this = *this*(1 / norm()); return *this; }

    T dot(const vec4<T> &v) const { return (x*v.x + y*v.y + z*v.z + w*v.w); }

    operator vec3<T> () const { return vec3<T>(x, y, z); }
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

    t det() { }
};


template <class t>
class matrix4x4
{
public:
    std::vector<vec4<t>> mat = std::vector<vec4<t>>(4);

    matrix4x4() {}
    matrix4x4(const matrix4x4<t> &o)
    {
        for (int i = 0; i < 4; i++) mat[i] = o.mat[i];
    }
    matrix4x4(const t (&a)[4][4])
    {
        for (int i = 0; i < 4; i++)
        {
            vec4<t> v(a[i][0], a[i][1], a[i][2], a[i][3]);
            mat[i] = v;
        }
    }

    matrix4x4<t>& operator += (const matrix4x4<t> &o)
    {
        for (int i = 0; i < 4; i++) mat[i] += o.mat[i];
        return *this;
    }

    matrix4x4<t> operator + (const matrix4x4<t> &o) const
    {
        matrix4x4<t> ret;
        for (int i = 0; i < 4; i++) ret.mat[i] = o.mat[i] + mat[i];
        return ret;
    }

    vec4<t> dot(const vec4<t> &v) const { return vec4<t>(mat[0].dot(v), mat[1].dot(v), mat[2].dot(v), mat[3].dot(v)); }

    matrix4x4<t> operator * (const matrix4x4<t> &o) const
    {
        matrix4x4<t> ret;
        vec4<t> cols[4] = {
            vec4<t>(o.mat[0].x, o.mat[1].x, o.mat[2].x, o.mat[3].x),
            vec4<t>(o.mat[0].y, o.mat[1].y, o.mat[2].y, o.mat[3].y),
            vec4<t>(o.mat[0].z, o.mat[1].z, o.mat[2].z, o.mat[3].z),
            vec4<t>(o.mat[0].w, o.mat[1].w, o.mat[2].w, o.mat[3].w)
        };
        for (int i = 0; i < 4; i++)
            ret.mat[i] = vec4<t>(mat[i].dot(cols[0]), mat[i].dot(cols[1]), mat[i].dot(cols[2]), mat[i].dot(cols[3]));
        return ret;
    }
};

template <int n>
class vec
{
    double v[n];

public:
    vec() { memset(v, 0, sizeof(v)); }
    vec(double (&a)[n]) { memcpy(v, a, sizeof(v)); }
    vec(const vec &o) { memcpy(v, o.v, sizeof(v)); }

    vec<n>& operator += (const vec<n> &o)  
    {
        for (int i = 0; i < n; i++) v[i] += o.v[i];
        return *this;
    }

    double dot(const vec<n> &o)
    {
        double ret = 0;
        for (int i = 0; i < n; i++) ret += o.v[i] * v[i];
        return ret;
    }
};