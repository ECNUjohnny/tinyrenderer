#include "tgaimage.h"


const int N = 2005;

struct vert_model
{
    float x, y, z; // coordinate for a triangle's vertex
    vert_model(): x{0}, y{0}, z{0} {}
    vert_model(const vert_model &o): x{o.x}, y{o.y}, z{o.z} {}
    vert_model(float x, float y, float z): x{x}, y{y}, z{z} {}
};

struct seg_model
{
    int i, j, k; // index for the vert_model array
    seg_model(): i{0}, j{0}, k{0} {}
    seg_model(int i, int j, int k): i{i}, j{j}, k{k} {}
};

class Model
{

    public:
        void draw(TGAImage &buffer);
        Model(char *file_name);
        int vcount(); // get the size of vert_modelex array
        int fcount(); // get the size of facet array
        vert_model vget(int i, int j); // get vert_modelex[facet[i].*], * depends on the value of j

    private:
        std::vector<vert_model> vertex;
        std::vector<seg_model> facet;
};