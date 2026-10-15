#include "our_gl.h"
#include <algorithm>
#include <vector>


matrix4x4<double> Viewport, Perspective, ModelView;
std::vector<double> zbuffer;

double signed_triangle_area(vec2<double> a, vec2<double> b, vec2<double> c)
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

void lookat(vec3<double> eye, vec3<double> center, vec3<double> up)
{
    vec3<double> n = (eye - center).normalize();
    vec3<double> l = (up ^ n).normalize();
    vec3<double> m = (n ^ l).normalize();

    ModelView = matrix4x4<double>{{{l.x,l.y,l.z,0}, {m.x,m.y,m.z,0}, {n.x,n.y,n.z,0}, {0,0,0,1}}} * 
                matrix4x4<double>{{{1,0,0,-center.x}, {0,1,0,-center.y}, {0,0,1,-center.z}, {0,0,0,1}}}; 
}

void rasterize(const Triangle &clip, const IShader &shader, TGAImage &framebuffer)
{
    vec4<double> ndc[3] = { clip[0] / clip[0].w, clip[1] / clip[1].w, clip[2] / clip[2].w };
    vec2<double> screen[3] = {
        Viewport.dot(ndc[0]),
        Viewport.dot(ndc[1]),
        Viewport.dot(ndc[2]),
    };
    
    double area = signed_triangle_area(screen[0], screen[1], screen[2]);

    if (area < 1) return;

    auto [bbminx, bbmaxx] = std::minmax({screen[0].x, screen[1].x, screen[2].x});
    auto [bbminy, bbmaxy] = std::minmax({screen[0].y, screen[1].y, screen[2].y});

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
            const vec2<double> p(x, y);
            double alpha = signed_triangle_area(screen[1], screen[2], p) / area;
            double beta = signed_triangle_area(screen[2], screen[0], p) / area;
            double gamma = signed_triangle_area(screen[0], screen[1], p) / area;
        
            if (alpha < 0 || beta < 0 || gamma < 0) continue;

            double z = alpha*ndc[0].z + beta*ndc[1].z + gamma*ndc[2].z;

            const int index = x + y * framebuffer.width();
            if (zbuffer[index] >= z) continue;

            auto [discard, color] = shader.fragment({alpha, beta, gamma});

            if (discard) continue;

            zbuffer[index] = z;

            framebuffer.set(x, y, color);
        }
    }
}