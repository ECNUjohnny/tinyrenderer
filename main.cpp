#include <iostream>
#include <cstring>
#include <cstdlib>
#include <tuple>
#include "model.h"
#include "tgaimage.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);

constexpr int width = 1024;
constexpr int height = 1024;

const char *name = "./diablo3_pose.obj";

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

vec2<int> viewport_projection(float x, float y)
{
    return vec2<int>((x + 1) * width / 2, (y + 1) * height / 2); 
}

vec3<float> barycentric(vec2<int> a, vec2<int> b, vec2<int> c, vec2<int> p)
{
    vec2<int> ab = b - a;
    vec2<int> ac = c - a;
    vec2<int> pa = a - p;

    vec3<float> cross = vec3<float>(ab.x, ac.x, pa.x) ^ vec3<float>(ab.y, ac.y, pa.y);
    return cross * (float)(1 / cross.z);
}

void traingle(vec2<int> a, vec2<int> b, vec2<int> c, TGAImage &buffer, TGAColor color)
{
    vec2<int> bbox[2]; // [1] is the biggest, [0] is the smallest
    bbox[1].x = std::max(a.x, std::max(b.x, c.x));
    bbox[1].y = std::max(a.y, std::max(b.y, c.y));
    bbox[0].x = std::min(a.x, std::min(b.x, c.x));
    bbox[0].y = std::min(a.y, std::min(b.y, c.y));

    for (int i = bbox[0].x; i <= bbox[1].x; i++)
    {
        for (int j = bbox[0].y; j <= bbox[1].y; j++)
        {
            vec3<float> bcoord = barycentric(a, b, c, vec2<int>(i, j));

            if (bcoord.x >= 0 && bcoord.y >= 0 && bcoord.x + bcoord.y <= 1) buffer.set(i, j, color);
        }
    }    
}


int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);

    vec2<int> a(250, 50); 
    vec2<int> b(100, 400); 
    vec2<int> c(325, 150); 

    Model model("./obj/african_head/african_head.obj");

    vec3<float> light_dir(0, 0, -1);

    for (int i = 0; i < model.fcount(); i++)
    {
        vec2<int> vert_triangle[3];
        vec3<float> vert_triangle_view[3];
        
        for (int j = 1; j <= 3; j++)
        {
            vert_triangle[j - 1] = viewport_projection(model.vget(i, j).x, model.vget(i, j).y);
            vert_triangle_view[j - 1] = vec3<float>(model.vget(i, j).x, model.vget(i, j).y, model.vget(i, j).z);
        }

        vec3<float> norm = (vert_triangle_view[2]-vert_triangle_view[0])^(vert_triangle_view[1]-vert_triangle_view[0]); 
        norm.normalize();

        float intensity = dot(norm, light_dir);

        if (intensity > 0)
        {
            traingle(
                vert_triangle[0], 
                vert_triangle[1], 
                vert_triangle[2], 
                framebuffer,
                TGAColor(intensity*255, intensity*255, intensity*255, 255) 
            );
        }
        
    }

    framebuffer.flip_vertically();
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
