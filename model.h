#include "tgaimage.h"


struct Vertex
{
    float x, y, z; // coordinates of a model vertex
    Vertex(): x{0}, y{0}, z{0} {}
    Vertex(const Vertex &o): x{o.x}, y{o.y}, z{o.z} {}
    Vertex(float x, float y, float z): x{x}, y{y}, z{z} {}
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
        Model(const char *file_name);
        int nverts() const; // number of vertices
        int nfaces() const; // number of faces (triangles)
        Vertex vert(int i) const; // the i-th vertex
        Face face(int i) const;   // the i-th face
        void draw(TGAImage &buffer);

    private:
        std::vector<Vertex> verts;
        std::vector<Face> faces;
};
