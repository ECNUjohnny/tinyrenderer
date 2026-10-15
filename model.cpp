#include <stdio.h>
#include "model.h"
#include "geometry.h"

Model::Model(const char *file_name)
{

    FILE *file = fopen(file_name, "r");
    char line[512];

    if (!file)
    {
        puts("Can't open the file");
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == 'v' && line[1] == ' ')
        {
            float x, y, z;

            sscanf(line + 2, "%f %f %f", &x, &y, &z);

            verts.push_back({x, y, z});
        }
        else if (line[0] == 'f' && line[1] == ' ')
        {
            int v, vt, vn;
            std::vector<int> corners;

            int offset = 2, chars = 0;

            while (sscanf(line + offset, "%d/%d/%d%n", &v, &vt, &vn, &chars) == 3)
            {
                corners.push_back(v - 1);
                offset += chars;
            }

            faces.push_back({corners[0], corners[1], corners[2]});
        }
    }

    fclose(file);
}


int Model::nverts() const
{
    return verts.size();
}

int Model::nfaces() const
{
    return faces.size();
}

Vertex Model::vert(int i) const
{
    return verts[i];
}

Face Model::face(int i) const
{
    return faces[i];
}

vec3<double> Model::get_vert_by_face_and_vert(int face, int vert) const
{

    switch (vert)
    {
    case 0:
        return verts[faces[face].v0];
        
    case 1:
        return verts[faces[face].v1];

    case 2:
        return verts[faces[face].v2];
    }

    return {};
}