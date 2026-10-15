#pragma once

#include "tgaimage.h"
#include "geometry.h"


struct Vertex
{
    float x, y, z; // coordinates of a model vertex
    Vertex(): x{0}, y{0}, z{0} {}
    Vertex(float x, float y, float z): x{x}, y{y}, z{z} {}

    operator vec3<double> () const { return {x, y, z}; }
};

struct Face
{
    int v0, v1, v2; // indices of the three corners into Model's vertex array
    Face(): v0{0}, v1{0}, v2{0} {}
    Face(int v0, int v1, int v2): v0{v0}, v1{v1}, v2{v2} {}
};

class Model
{

    public:
        Model(const char *model_name, const char *normalmap_name, const char *texture_name);
        int nverts() const; // number of vertices
        int nfaces() const; // number of faces (triangles)
        Vertex vert(int i) const;
        Vertex vert(int iface, int nthvert) const; // the i-th vertex
        vec3<double> norm(int iface, int nthvert) const; // object-space normal; nthvert is 0, 1 or 2
        vec2<double> uv(int iface, int nthvert) const; // texture coordinates; nthvert is 0, 1 or 2
        TGAColor tex(vec2<double> uv) const;
        vec3<double> norm_uv(vec2<double> uv) const;

    private:
        std::vector<Vertex> verts;
        std::vector<vec3<double>> norms;
        std::vector<vec2<double>> uvs;
        std::vector<int> faces_vrt;
        std::vector<int> faces_nrm;
        std::vector<int> faces_tex;
        TGAImage normalmap = {};
        TGAImage texture = {};
};


