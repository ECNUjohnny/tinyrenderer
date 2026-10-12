#include <iostream>
#include <cstring>
#include <tuple>
#include "model.h"
#include "tgaimage.h"

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
    
    if (abs(ay - by) > abs(ax - bx))
    {
        std::swap(ax, ay), std::swap(bx, by);
        flag = 0;
    }    
    
    if (ax > ay)
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

std::tuple<int, int> project(vert v)
{
    return {(v.x + 1) / 2 * width, (v.y + 1) / 2 * height};
}

void traingle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &buffer, TGAColor color)
{
    line(ax, ay, bx, by, buffer, color); 
    line(bx, by, cx, cy, buffer, color); 
    line(cx, cy, ax, ay, buffer, color); 

}

int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);

    traingle(50, 50, 200, 100, 425, 150, framebuffer, red); 
    
    

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
