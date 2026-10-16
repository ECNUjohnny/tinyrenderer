#pragma once

#include "tgaimage.h"
#include "geometry.h"


using Vertex = vec<3>;

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
        vec<3> norm(int iface, int nthvert) const; // object-space normal; nthvert is 0, 1 or 2
        vec<2> uv(int iface, int nthvert) const; // texture coordinates; nthvert is 0, 1 or 2
        TGAColor tex(vec<2> uv) const;
        vec<3> norm_uv(vec<2> uv) const;

    private:
        std::vector<Vertex> verts;
        std::vector<vec<3>> norms;
        std::vector<vec<2>> uvs;
        std::vector<int> faces_vrt;
        std::vector<int> faces_nrm;
        std::vector<int> faces_tex;
        TGAImage normalmap = {};
        TGAImage texture = {};
};


