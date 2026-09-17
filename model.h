#include "tgaimage.h"


const int N = 2005;

struct vert
{
    float x, y, z; // coordinate for a triangle
    vert(): x{0}, y{0}, z{0} {}
    vert(const vert &o): x{o.x}, y{o.y}, z{o.z} {}
    vert(float x, float y, float z): x{x}, y{y}, z{z} {}
};

struct seg
{
    int i, j, k; // index for the vertex array
    seg(): i{0}, j{0}, k{0} {}
    seg(int i, int j, int k): i{i}, j{j}, k{k} {}
};

class Model
{

    public:
        void draw(TGAImage &buffer);
        Model(char *file_name);
        int vcount(); // get the size of vertex array
        int fcount(); // get the size of facet array
        vert vget(int i, int j); // get vertex[facet[i].*], * depends on the value of j

    private:
        std::vector<vert> vertex;
        std::vector<seg> facet;
};