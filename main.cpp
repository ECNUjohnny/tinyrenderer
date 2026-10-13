#include <iostream>
#include <cstring>
#include <cstdlib>
#include <tuple>
#include "model.h"
#include "tgaimage.h"
#include "geometry.h"

const TGAColor white = {255, 255, 255, 255};
const TGAColor red   = {255, 0,   0,   255};
const TGAColor green = {0,   255, 0,   255};

constexpr int width = 1024;
constexpr int height = 1024;


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

double signed_triangle_area(vec2<int> a, vec2<int> b, vec2<int> c)
{
    return 0.5 * ((b - a) ^ (c - a));
}

vec3<int> viewport_projection(vec3<double> v)
{
    return vec3<int>((v.x+1)*width/2, (v.y+1)*height/2, (v.z+1)*255/2); 
}

void traingle(vec3<int> a, vec3<int> b, vec3<int> c, TGAImage &buffer, TGAImage &zbuffer, TGAColor color)
{
    vec2<int> bbox[2]; // [1] is the biggest, [0] is the smallest
    bbox[1].x = std::max(a.x, std::max(b.x, c.x));
    bbox[1].y = std::max(a.y, std::max(b.y, c.y));
    bbox[0].x = std::min(a.x, std::min(b.x, c.x));
    bbox[0].y = std::min(a.y, std::min(b.y, c.y));
    double total_area = signed_triangle_area(a, b, c);

    if (total_area < 1) return;

    for (int i = bbox[0].x; i <= bbox[1].x; i++)
    {
        for (int j = bbox[0].y; j <= bbox[1].y; j++)
        {
            double alpha = signed_triangle_area(b, c, {i, j}) / total_area;
            double beta = signed_triangle_area(c, a, {i, j}) / total_area;
            double gamma = signed_triangle_area(a, b, {i, j}) / total_area;
           
            if (alpha < 0 || beta < 0 || gamma < 0) continue;
            
            int z = alpha * a.z + beta * b.z + gamma * c.z;
            
            zbuffer.set(i, j, {z});
            buffer.set(i, j, color); 
        }
    }    
}


int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    vec2<int> a(250, 50); 
    vec2<int> b(100, 400); 
    vec2<int> c(325, 150); 

    Model model("./diablo3_pose.obj");

    vec3<double> light_dir(0, 0, -1);

    for (int i = 0; i < model.fcount(); i++)
    {
        vec3<int> a = viewport_projection({model.vget(i, 1).x, model.vget(i, 1).y, model.vget(i, 1).z}); 
        vec3<int> b = viewport_projection({model.vget(i, 2).x, model.vget(i, 2).y, model.vget(i, 2).z}); 
        vec3<int> c = viewport_projection({model.vget(i, 3).x, model.vget(i, 3).y, model.vget(i, 3).z}); 
        
        traingle(a, b, c, framebuffer, zbuffer, {rand() % 225, rand() % 225, rand() % 225, 255});
    }

    framebuffer.write_tga_file("framebuffer.tga");
    zbuffer.write_tga_file("zbuffer.tga");
    
    return 0;
}
