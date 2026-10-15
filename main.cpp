#include <cstring>
#include <cstdlib>
#include <iostream>
#include <algorithm>
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

const char *african = "obj/african_head/african_head.obj";
const char *diablo3 = "./diablo3_pose.obj";

extern matrix4x4<double> Viewport, Perspective, ModelView;
extern std::vector<double> zbuffer;

struct Shader: IShader
{
    const Model &model;
    TGAColor color = {};
    vec3<double> tri[3];  // triangle in eye coordinates

    Shader(const Model &m) : model(m) {}

    virtual vec4<double> vertex(const int face, const int vert) 
    {
        vec3<double> v = model.get_vert_by_face_and_vert(face, vert);                          // current vertex in object coordinates
        vec4<double> gl_Position = ModelView.dot(vec4{v.x, v.y, v.z, 1.});
        tri[vert] = gl_Position;                            // in eye coordinates
        return Perspective.dot(gl_Position);                         // in clip coordinates
    }

    virtual std::pair<bool,TGAColor> fragment(const vec3<double> bar) const 
    {
        return {false, color};                                    // do not discard the pixel
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

vec3<double> rot(vec3<double> vert)
{
    double a = std::acos(-1.0) / 6;
    matrix3x3<double> rot_mat({{std::cos(a), 0, std::sin(a)}, {0,1,0}, {-std::sin(a), 0, std::cos(a)}});
    return rot_mat.dot(vert);  
}



int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);
    init_zbuffer(width, height);

    Model model(african);
    Shader shader(model);

    vec3<double> light_dir(0, 0, -1);

    lookat(eye, center, up);
    init_perspective((eye-center).norm());
    init_viewport(width/16, height/16, width*7/8, height*7/8); 


    for (int i = 0; i < model.nfaces(); i++)
    {
        Face f = model.face(i);
        vec4<double> va(model.vert(f.v0).x, model.vert(f.v0).y, model.vert(f.v0).z, 1);
        vec4<double> vb(model.vert(f.v1).x, model.vert(f.v1).y, model.vert(f.v1).z, 1);
        vec4<double> vc(model.vert(f.v2).x, model.vert(f.v2).y, model.vert(f.v2).z, 1);
        
        vec4<double> clip[3];
        clip[0] = Perspective.dot(ModelView.dot(va));
        clip[1] = Perspective.dot(ModelView.dot(vb));
        clip[2] = Perspective.dot(ModelView.dot(vc));

        const TGAColor color = {
            static_cast<std::uint8_t>(std::rand() % 255),
            static_cast<std::uint8_t>(std::rand() % 255),
            static_cast<std::uint8_t>(std::rand() % 255),
            255
        };
        shader.color = color;
        rasterize(clip, shader, framebuffer);
    }

    framebuffer.write_tga_file("framebuffer.tga");
    
    return 0;
}
