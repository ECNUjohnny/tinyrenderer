#include <cstring>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "model.h"
#include "tgaimage.h"
#include "our_gl.h"

const TGAColor white = {255, 255, 255, 255};
const TGAColor red   = {255, 0,   0,   255};
const TGAColor green = {0,   255, 0,   255};

constexpr int width = 1024;
constexpr int height = 1024;
const vec3<double>    eye{-1,0,2};
const vec3<double> center{0,0,0};
const vec3<double>     up{0,1,0};

const char *african = "./obj/african_head/african_head.obj";
const char *diablo3 = "./diablo3_pose.obj";

const char *african_norm = "./obj/african_head/african_head_nm.tga";
const char *diablo3_norm = "./";

const char *african_tex = "./obj/african_head/african_head_diffuse.tga";
const char *diablo3_tex = "./obj/diablo3_pose/diablo3_pose_diffuse.tga";

extern matrix4x4<double> Viewport, Perspective, ModelView;
extern std::vector<double> zbuffer;

struct Shader: IShader
{
    const Model &model;
    TGAColor color = {255, 255, 255, 255};
    vec3<double> tri[3];  // triangle in eye coordinates
    vec3<double> tri_norm[3]; // normal on three vertex
    vec2<double> tri_uv[3];
    vec3<double> l; 

    Shader(const Model &m, vec4<double> light) : model(m)
    {
        l = ModelView.dot(light).normalize();
    }

    virtual vec4<double> vertex(const int face, const int vert) 
    {
        vec3<double> v = model.vert(face, vert);                          // current vertex in object coordinates
        vec3<double> n = model.norm(face, vert);
        vec2<double> uv = model.uv(face, vert);  

        vec4<double> gl_Position = ModelView.dot(vec4{v.x, v.y, v.z, 1.});
        vec4<double> gl_Normal = ModelView.dot(vec4{n.x, n.y, n.z, 0.});        

        tri[vert] = gl_Position;                           
        tri_norm[vert] = gl_Normal;
        tri_uv[vert] = uv;

        return Perspective.dot(gl_Position);                         
    }

    virtual std::pair<bool, TGAColor> fragment(const vec3<double> bar) const 
    {
        auto [a, b, c] = bar;
        
        vec2<double> uv = tri_uv[0]*a + tri_uv[1]*b + tri_uv[2]*c;
        vec3<double> n = model.norm_uv(uv);
        // vec3<double> n = tri_norm[0]*a + tri_norm[1]*b + tri_norm[2]*c;

        // This normal map is in object space; the light is in eye space.
        n = ModelView.dot(vec4<double>{n.x, n.y, n.z, 0.});
        n.normalize();

        double e = 25;

        double ambient = 0.3;

        vec3<double> r = n*2*n.dot(l) - l;

        double diff = std::max(0., n.dot(l)); 

        double spec = std::pow(std::max(r.z, 0.), e);

        TGAColor gl_color = model.tex(uv);
        for (int i = 0; i <= 2; i++) gl_color[i] *= std::min(1., ambient + .4 * diff + .9 * spec);

        return {false, gl_color};                                    // do not discard the pixel
    }    
};

// line drawing algorithm

void line(vec2<int> a, vec2<int> b, TGAImage &buffer, TGAColor color)
{
    const bool steep = std::abs(a.y - b.y) > std::abs(a.x - b.x);
    if (steep)
    {
        std::swap(a.x, a.y);
        std::swap(b.x, b.y);
    }

    if (a.x > b.x)
    {
        std::swap(a, b);
    }

    int y = a.y, ierror = 0;

    for (int x = a.x; x <= b.x; x++)
    {
        if (steep) buffer.set(y, x, color);
        else buffer.set(x, y, color);

        ierror += 2 * std::abs(b.y - a.y);
        if (ierror > b.x - a.x)
        {
            y += b.y > a.y ? 1 : -1;
            ierror -= 2 * (b.x - a.x);
        }
    }
}



int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);

    init_zbuffer(width, height);

    Model model(african, african_norm, african_tex);

    lookat(eye, center, up);
    init_perspective((eye-center).norm());
    init_viewport(width/16, height/16, width*7/8, height*7/8); 

    Shader shader(model, {0, 1, 1, 0});

    for (int i = 0; i < model.nfaces(); i++)
    { 
        vec4<double> clip[3] = { 
            shader.vertex(i, 0),  
            shader.vertex(i, 1),
            shader.vertex(i, 2)
        };

        rasterize(clip, shader, framebuffer);
    }

    framebuffer.write_tga_file("framebuffer.tga");
    
    return 0;
}
