#include <iostream>
#include <cstring>
#include "tgaimage.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

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

struct vertix
{
    float x, y, z;
    vertix(float x, float y, float z): x{x}, y{y}, z{z} {}
};

int tot, ptr;
FILE *model;
vertix v[5005];
char dats[105], dat1[15], dat2[15], dat3[15];

int main(int argc, char** argv) {
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);


    int ax =  7, ay =  3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;

    framebuffer.set(ax, ay, red);
    framebuffer.set(bx, by, blue);
    framebuffer.set(cx, cy, green);

    model = fopen("./diablo3_pose.obj", "r");       

    while (fgets(dats + 1, sizeof(line), model))
    {
        tot++, ptr = 2;
        
        if (dats[1] == 'v')
        {
                   
        } 
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
