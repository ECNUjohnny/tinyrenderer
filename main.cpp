#include <cstring>
#include <cstdlib>
#include <iostream>
#include <iostream>
#include <algorithm>
#include "model.h"
#include "tgaimage.h"
#include "geometry.h"

const TGAColor white = {255, 255, 255, 255};
const TGAColor red   = {255, 0,   0,   255};
const TGAColor green = {0,   255, 0,   255};

constexpr int width = 1024;
constexpr int height = 1024;

const char *african = "obj/african_head/african_head.obj";
const char *diablo3 = "./diablo3_pose.obj";

matrix4x4<double> Viewport, Perspective, ModelView;

void line(int ax, int ay, int bx, int by, TGAImage &buffer, TGAColor color)
{
    int flag = 1;
    
    if (std::abs(ay - by) > std::abs(ax - bx))
    {
        std::swap(ax, ay), std::swap(bx, by);
        flag = 0;
    }    
    
    if (ax > bx)
    {
        std::swap(ax, bx), std::swap(ay, by);
    }    

    int j = ay, ierror = 0;

    for (int i = ax; i <= bx; i++)
    {
        if (!flag) buffer.set(j, i, color);
        else buffer.set(i, j, color);

        ierror += 2 * std::abs(by - ay);
        
        j += (by > ay ? 1 : -1 ) * (ierror > bx - ax); 
        ierror -= 2 * (bx - ax) * (ierror > bx - ax);
    }
}

vec3<double> rot(vec3<double> vert)
{
    double a = M_PI / 6;
    matrix3x3<double> rot_mat({{std::cos(a), 0, std::sin(a)}, {0,1,0}, {-std::sin(a), 0, std::cos(a)}});
    return rot_mat.dot(vert);  
}

double signed_triangle_area(vec2<double> a, vec2<double> b, vec2<double> c)
{
    return 0.5 * ((b - a) ^ (c - a));
}

void viewport(int x, int y, int w, int h)
{
    Viewport = {{{w/2., 0, 0, x+w/2.}, {0, h/2., 0, y+h/2.}, {0,0,1,0}, {0,0,0,1}}};
}

void perspective(const double f) 
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

void rasterize(vec4<double> clip[3], TGAImage &buffer, TGAImage &zbuffer, TGAColor color)
{
    vec4<double> ndc[3] = { clip[0] / clip[0].w, clip[1] / clip[1].w, clip[2] / clip[2].w };
    vec2<double> screen[3] = {
        Viewport.dot(ndc[0]),
        Viewport.dot(ndc[1]),
        Viewport.dot(ndc[2]),
    };
    
    matrix3x3<double> ABC = {{ {screen[0].x, screen[0].y, 1.}, {screen[1].x, screen[1].y, 1.}, {screen[2].x, screen[2].y, 1.} }};
    
    double area = signed_triangle_area(screen[0], screen[1], screen[2]);

    if (area < 1) return;

    auto [bbminx, bbmaxx] = std::minmax({screen[0].x, screen[1].x, screen[2].x});
    auto [bbminy, bbmaxy] = std::minmax({screen[0].y, screen[1].y, screen[2].y});

    for (int x = bbminx; x <= bbmaxx; x++)
    {
        for (int y = bbminy; y <= bbmaxy; y++)
        {
            double alpha = signed_triangle_area(screen[1], screen[2], {x, y}); 
            double beta = signed_triangle_area(screen[2], screen[0], {x, y}); 
            double gamma = signed_triangle_area(screen[0], screen[1], {x, y}); 
        
            if (alpha < 0 || beta < 0 || gamma < 0) return;

            
        }
    }
}


int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE); 

    Model model(african);

    vec3<double> light_dir(0, 0, -1);

    for (int i = 0; i < model.nfaces(); i++)
    {
        Face f = model.face(i);
        vec3<double> va(model.vert(f.v0).x, model.vert(f.v0).y, model.vert(f.v0).z);
        vec3<double> vb(model.vert(f.v1).x, model.vert(f.v1).y, model.vert(f.v1).z);
        vec3<double> vc(model.vert(f.v2).x, model.vert(f.v2).y, model.vert(f.v2).z);
        
         
        
    }

    framebuffer.write_tga_file("framebuffer.tga");
    zbuffer.write_tga_file("zbuffer.tga");
    
    return 0;
}
