#include <iostream>
#include <cstring>
#include <tuple>
#include "model.h"
#include "tgaimage.h"
#include "geometry.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

constexpr int width = 512;
constexpr int height = 512;

char *name = "./diablo3_pose.obj";

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

template <class t>
vec2<t> viewport_projection(float x, float y)
{
    
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

    for (int i = 0; i < model.fcount(); i++)
    {

    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
