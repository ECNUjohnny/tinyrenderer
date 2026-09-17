#include "model.h"
#include <cstring>

Model::Model(char *file_name)
{
    
    FILE *file = fopen(file_name, "r");
    char datas[512];

    if (!file)
    {
        puts("Can't open the file");
        return;
    }

    while (fgets(datas, sizeof(datas), file))
    {
        if (datas[0] == 'v' && datas[1] == ' ')
        {
            float x, y, z;

            sscanf(datas + 2, "%f %f %f", &x, &y, &z);
            
            vertex.push_back({x, y, z});
        }
        else if (datas[0] == 'f' && datas[1] == ' ')
        {
            int f, t, n;
            std::vector<int> fs;

            int offset = 2, chars = 0;
            
            while (sscanf(datas + offset, "%d/%d/%d%n", &f, &t, &n, &chars) == 3)
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