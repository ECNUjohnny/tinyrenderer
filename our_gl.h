#include "tgaimage.h"
#include "geometry.h"

void lookat(const vec<3> eye, const vec<3> center, const vec<3> up);
void init_perspective(const double f);
void init_viewport(const int x, const int y, const int w, const int h);
void init_zbuffer(const int width, const int height);

struct IShader 
{
    static TGAColor sample2D(const TGAImage &img, vec<2> uv)
    {
        return img.get(uv[0]*img.width(), uv[1]*img.height());
    }
    
    virtual std::pair<bool,TGAColor> fragment(const vec<3> bar) const = 0;
};

typedef vec<4> Triangle[3]; // a triangle primitive is made of three ordered points
void rasterize(const Triangle &clip, const IShader &shader, TGAImage &framebuffer);