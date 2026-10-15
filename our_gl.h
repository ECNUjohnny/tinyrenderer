#include "tgaimage.h"
#include "geometry.h"

void lookat(const vec3<double> eye, const vec3<double> center, const vec3<double> up);
void init_perspective(const double f);
void init_viewport(const int x, const int y, const int w, const int h);
void init_zbuffer(const int width, const int height);

struct IShader 
{
    virtual std::pair<bool,TGAColor> fragment(const vec3<double> bar) const = 0;
};

typedef vec4<double> Triangle[3]; // a triangle primitive is made of three ordered points
void rasterize(const Triangle &clip, const IShader &shader, TGAImage &framebuffer);