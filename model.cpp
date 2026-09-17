#include "model.h"
#include <cstring>

Model::Model(char *file_name)
{
    
    FILE *file = fopen(file_name, "r");
    char datas[305];

    while (fgets(datas + 1, sizeof(datas), file))
    {
        if (datas[1] == 'v')
        {
            float x, y, z;

            sscanf(datas + 3, "%f %f %f", &x, &y, &z);
            
            vertex.push_back({x, y, z});
        }
        else if (datas[1] == 'f')
        {
            int f, t, n;
            std::vector<int> fs;

            int offset = 3, chars = 0;
            
            while (sscanf(datas + offset, "%d/%d/%d%n", &f, &t, &n, &chars))
            {
                fs.push_back(f - 1);
                offset += chars;
            }

            facet.push_back({fs[0], fs[1], fs[2]});
        }
    }

    fclose(file);
}


void Model::draw(TGAImage &buffer)
{

}

int Model::vcount()
{
    return vertex.size();
}

int Model::fcount()
{
    return facet.size();
}

vert Model::vget(int i, int j)
{
    switch(j)
    {
        case 1:
            return vertex[facet[i].i];
            break;
        
        case 2:
            return vertex[facet[i].j];
            break;

        case 3:
            return vertex[facet[i].k];
            break;
    }
}