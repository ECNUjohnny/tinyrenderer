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

constexpr int shadoww = 8000;    // shadow map buffer size
constexpr int shadowh = 8000;
constexpr int width = 1024;
constexpr int height = 1024;
const vec<4>  light{1, 1, 1, 0};
const vec<3>    eye{-0.5,0,2};
const vec<3> center{0,0,0};
const vec<3>     up{0,1,0};

char *african = "./obj/african_head/african_head.obj";
char *diablo3 = "./diablo3_pose.obj";
char *floor_model = "./obj/floor.obj";

char *african_norm = "./obj/african_head/african_head_nm_tangent.tga";
char *diablo3_norm = "./obj/diablo3_pose/diablo3_pose_nm_tangent.tga";
char *floor_norm = "./obj/floor_nm_tangent.tga";

char *african_tex = "./obj/african_head/african_head_diffuse.tga";
char *diablo3_tex = "./obj/diablo3_pose/diablo3_pose_diffuse.tga";
char *floor_tex = "./obj/floor_diffuse.tga";

char *model_name[10];
char *normalmap_name[10];
char *uvmap_name[10];

extern matrix<4, 4> Viewport, Perspective;
extern matrix<4, 4> ModelView;
extern std::vector<double> zbuffer;

struct BlankShader: IShader
{
    const Model &model;

    BlankShader(const Model &o): model(o) {}

    virtual vec<4> vertex(const int face, const int vert)
    {
        vec<3> v = model.vert(face, vert);
        vec<4> gl_Position = ModelView * vec<4>(v[0], v[1], v[2], 1.);
        return Perspective * gl_Position;
    }

    virtual std::pair<bool, TGAColor> fragment(const vec<3> bar) const
    {
        return {false, {255, 255, 255, 255}};
    }
};

struct Shader: IShader
{
    const Model &model;
    TGAColor color = {255, 255, 255, 255};
    vec<3> tri[3];  // triangle in eye coordinates
    vec<3> tri_norm[3]; // normal on three vertex
    vec<2> tri_uv[3];
    vec<3> l; 

    Shader(const Model &m, vec<4> light) : model(m)
    {
        l = ModelView.matmul(light).normalize();
    }

    virtual vec<4> vertex(const int face, const int vert) 
    {
        vec<3> v = model.vert(face, vert);                          // current vertex in object coordinates
        vec<3> n = model.norm(face, vert);
        vec<2> uv = model.uv(face, vert);  

        vec<4> gl_Position = ModelView.matmul(vec<4>{v[0], v[1], v[2], 1.});
        vec<4> gl_Normal = ModelView.invert_transpose().matmul(vec<4>{n[0], n[1], n[2], 0.});         

        tri[vert] = gl_Position;                           
        tri_norm[vert] = gl_Normal;
        tri_uv[vert] = uv;

        return Perspective.matmul(gl_Position);                         
    }

    virtual std::pair<bool, TGAColor> fragment(const vec<3> bar) const 
    {
        const double a = bar[0], b = bar[1], c = bar[2];
        
        vec<2> uv = tri_uv[0]*a + tri_uv[1]*b + tri_uv[2]*c;
        vec<3> uvn = model.norm_uv(uv);
        vec<3> n = tri_norm[0]*a + tri_norm[1]*b + tri_norm[2]*c;

        // This normal map is in object space; the light is in eye space.
        // n = ModelView.invert_transpose().matmul(vec<4>{n[0], n[1], n[2], 0.});
        n.normalize();

        matrix<2, 3> E;
        E[0] = tri[1] - tri[0], E[1] = tri[2] - tri[0];        

        matrix<2, 2> U;
        U[0] = tri_uv[1] - tri_uv[0], U[1] = tri_uv[2] - tri_uv[0];

        matrix<2, 3> T = U.invert() * E;
        matrix<3, 3> tbn;
        tbn[0] = T[0].normalize(), tbn[1] = T[1].normalize(), tbn[2] = n;
        
        n = (tbn.transpose() * uvn).normalize();

        double e = 35;
        double ambient = 0.3;

        vec<3> r = n*2*n.dot(l) - l;

        double diff = std::max(0., n.dot(l)); 
        double spec = std::pow(std::max(r[2], 0.), e);

        TGAColor gl_color = model.tex(uv);
        for (int i = 0; i <= 2; i++) gl_color[i] *= std::min(1., ambient + .4 * diff + .9 * spec);

        return {false, gl_color};                                    // do not discard the pixel
    }    
};

// line drawing algorithm

void line(vec<2> a, vec<2> b, TGAImage &buffer, TGAColor color)
{
    // Raster coordinates remain integers even though vec stores doubles.
    for (int i = 0; i < 2; i++) {
        a[i] = static_cast<int>(a[i]);
        b[i] = static_cast<int>(b[i]);
    }
    const bool steep = std::abs(a[1] - b[1]) > std::abs(a[0] - b[0]);
    if (steep)
    {
        std::swap(a[0], a[1]);
        std::swap(b[0], b[1]);
    }

    if (a[0] > b[0])
    {
        std::swap(a, b);
    }

    int y = a[1], ierror = 0;

    for (int x = a[0]; x <= b[0]; x++)
    {
        if (steep) buffer.set(y, x, color);
        else buffer.set(x, y, color);

        ierror += 2 * std::abs(b[1] - a[1]);
        if (ierror > b[0] - a[0])
        {
            y += b[1] > a[1] ? 1 : -1;
            ierror -= 2 * (b[0] - a[0]);
        }
    }
}

void drop_zbuffer(char *filename, std::vector<double> &zbuffer, int width, int height)
{
    double minz = 1000;
    double maxz = -1000;
    
    TGAImage zimg(width, height, TGAImage::GRAYSCALE, {0, 0, 0, 0});

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            double z = zbuffer[x+y*width];
            if (z < -100) continue;
            minz = std::min(minz, z);
            maxz = std::max(maxz, z);
        }
    }

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            double z = zbuffer[x+y*width];
            if (z < -100) continue;
            z = (z - minz) / (maxz - minz) * 255;
            zimg.set(x, y, {(unsigned char)z, 255, 255, 255});
        }
    }

    zimg.write_tga_file(filename);
}

int main(int argc, char **argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);
    for (int i = 0; i < framebuffer.width(); i++) {
        for (int j = 0; j < framebuffer.height(); j++) framebuffer.set(i, j, {56, 128, 128, 255});
    }

    model_name[0] = diablo3, normalmap_name[0] = diablo3_norm, uvmap_name[0] = diablo3_tex;
    model_name[1] = floor_model, normalmap_name[1] = floor_norm, uvmap_name[1] = floor_tex;

    init_zbuffer(width, height);
    lookat(eye, center, up);
    init_perspective((eye-center).norm());
    init_viewport(width/16, height/16, width*7/8, height*7/8); 

    for (int j = 0; j < 2; j++)
    {
        Model model(model_name[j], normalmap_name[j], uvmap_name[j]); 
        Shader shader(model, light); // here use the light direction

        for (int i = 0; i < model.nfaces(); i++)
        { 
            vec<4> clip[3] = { 
                shader.vertex(i, 0),  
                shader.vertex(i, 1),
                shader.vertex(i, 2)
            };

            rasterize(clip, shader, framebuffer);
        }
    }

    framebuffer.write_tga_file("framebuffer.tga");
    drop_zbuffer("zbuffer1.tga", zbuffer, width, height);
    
    std::vector<bool> mask(width*height, false);
    std::vector<double> zbuffer_copy = zbuffer;
    matrix<4,4> M = (Viewport * Perspective * ModelView).invert();

    {
        lookat(light, center, up);
        init_perspective((eye-center).norm());
        init_viewport(shadoww/16, shadowh/16, shadoww*7/8, shadowh*7/8);    
        init_zbuffer(shadoww, shadowh);
        TGAImage trash(shadoww, shadowh, TGAImage::RGB, {177, 195, 209, 255});
        
        for (int i = 0; i < 2; i++) {
            Model model(model_name[i], normalmap_name[i], uvmap_name[i]);
            BlankShader bshader{model};

            for (int i = 0; i < model.nfaces(); i++)
            {
                vec<4> clip[3] = {
                    bshader.vertex(i, 0),
                    bshader.vertex(i, 1),
                    bshader.vertex(i, 2)
                };

                rasterize(clip, bshader, trash);
            } 
        }

        trash.write_tga_file("shadowmap.tga");        
    }

    drop_zbuffer("zbuffer2.tga", zbuffer, shadoww, shadowh);

    matrix<4, 4> N = Viewport * Perspective * ModelView;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            vec<4> fragment = M * vec<4>{x, y, zbuffer_copy[x+y*width], 1.};
            vec<4> q = N * fragment;
            q /= q[3];
            bool lit = (fragment[2] < -100 || (q[0] < 0 || q[0] >= shadoww || q[1] < 0
                || q[1] > shadowh) || (q[2] > zbuffer[int(q[0]) + int(q[1])*shadoww] - .03)); 
        
            mask[x+y*width] = lit;
        }
    }

    TGAImage maskimg(width, height, TGAImage::GRAYSCALE);
    for (int x=0; x<width; x++) {
        for (int y=0; y<height; y++) {
            if (mask[x+y*width]) continue;
            maskimg.set(x, y, {255, 255, 255, 255});
        }
    }
    maskimg.write_tga_file("mask.tga");

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if (mask[x+y*width]) continue;
            TGAColor c = framebuffer.get(x, y);
            vec<3> a = {c[0], c[1], c[2]};
            if (a.norm() < 80) continue;
            a = a.normalize() * 80;
            framebuffer.set(x, y, {(unsigned char)a[0], (unsigned char)a[1], (unsigned char)a[2], 255});
        }
    } 

    framebuffer.write_tga_file("shadow.tga");

    return 0;
}
