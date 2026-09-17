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

constexpr int width = 800;
constexpr int height = 800;

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

int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);

    Model model(name);

    for (int i = 0; i < model.fcount(); i++)
    {
        auto [ax, ay] = project(model.vget(i, 1));
        auto [bx, by] = project(model.vget(i, 2));
        auto [cx, cy] = project(model.vget(i, 3));

        line(ax, ay, bx, by, framebuffer, red);
        line(bx, by, cx, cy, framebuffer, red);
        line(cx, cy, ax, ay, framebuffer, red);
    }
    

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
