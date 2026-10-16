#include "our_gl.h"
#include <algorithm>
#include <vector>


matrix<4, 4> Viewport, Perspective, ModelView;
std::vector<double> zbuffer;

double signed_triangle_area(vec<2> a, vec<2> b, vec<2> c)
{
    return 0.5 * ((b - a) ^ (c - a));
}

void init_zbuffer(int width, int height)
{
    zbuffer.resize(width*height, -1e9);    
}

void init_viewport(int x, int y, int w, int h)
{
    Viewport = {{{w/2., 0, 0, x+w/2.}, {0, h/2., 0, y+h/2.}, {0,0,1,0}, {0,0,0,1}}};
}

void init_perspective(const double f) 
{
    Perspective = {{{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0, -1/f,1}}};
}

void lookat(vec<3> eye, vec<3> center, vec<3> up)
{
    vec<3> n = (eye - center).normalize();
    vec<3> l = (up ^ n).normalize();
    vec<3> m = (n ^ l).normalize();

    ModelView = matrix<4, 4>{{{l[0],l[1],l[2],0}, {m[0],m[1],m[2],0}, {n[0],n[1],n[2],0}, {0,0,0,1}}} * 
                matrix<4, 4>{{{1,0,0,-center[0]}, {0,1,0,-center[1]}, {0,0,1,-center[2]}, {0,0,0,1}}}; 
}

void rasterize(const Triangle &clip, const IShader &shader, TGAImage &framebuffer)
{
    vec<4> ndc[3] = { clip[0] / clip[0][3], clip[1] / clip[1][3], clip[2] / clip[2][3] };
    vec<2> screen[3] = {
        Viewport.matmul(ndc[0]),
        Viewport.matmul(ndc[1]),
        Viewport.matmul(ndc[2]),
    };
    
    double area = signed_triangle_area(screen[0], screen[1], screen[2]);

    if (area < 1) return;

    auto [bbminx, bbmaxx] = std::minmax({screen[0][0], screen[1][0], screen[2][0]});
    auto [bbminy, bbmaxy] = std::minmax({screen[0][1], screen[1][1], screen[2][1]});

    // Clip the pixel bounds before indexing the depth buffer.
    bbminx = std::max(0.0, std::ceil(bbminx));
    bbminy = std::max(0.0, std::ceil(bbminy));
    bbmaxx = std::min(double(framebuffer.width() - 1), std::floor(bbmaxx));
    bbmaxy = std::min(double(framebuffer.height() - 1), std::floor(bbmaxy));
    if (bbminx > bbmaxx || bbminy > bbmaxy) return;

    for (int x = int(bbminx); x <= int(bbmaxx); x++)
    {
        for (int y = int(bbminy); y <= int(bbmaxy); y++)
        {
            const vec<2> p(x, y);
            double alpha = signed_triangle_area(screen[1], screen[2], p) / area;
            double beta = signed_triangle_area(screen[2], screen[0], p) / area;
            double gamma = signed_triangle_area(screen[0], screen[1], p) / area;
        
            if (alpha < 0 || beta < 0 || gamma < 0) continue;

            double z = alpha*ndc[0][2] + beta*ndc[1][2] + gamma*ndc[2][2];

            const int index = x + y * framebuffer.width();
            if (zbuffer[index] >= z) continue;

            vec<3> bar = {alpha / clip[0][3], beta / clip[1][3], gamma / clip[2][3]};
            bar = bar / (bar[0] + bar[1] + bar[2]);
            auto [discard, color] = shader.fragment(bar);

            if (discard) continue;

            zbuffer[index] = z;

            framebuffer.set(x, y, color);
        }
    }
}