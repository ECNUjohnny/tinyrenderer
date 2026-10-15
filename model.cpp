#include <stdio.h>
#include <algorithm>
#include "model.h"
#include "geometry.h"

Model::Model(const char *model_name, const char *normalmap_name, const char *texture_name)
{

    FILE *file = fopen(model_name, "r");
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
        else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
        {
            double x, y, z;

            if (sscanf(line + 3, "%lf %lf %lf", &x, &y, &z) == 3)
            {
                norms.push_back({x, y, z});
            }
        }
        else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
        {
            double u, v;

            // Use the first two components for 2D textures; ignore optional w.
            if (sscanf(line + 3, "%lf %lf", &u, &v) == 2)
            {
                uvs.push_back({u, v});
            }
        }
        else if (line[0] == 'f' && line[1] == ' ')
        {
            int v, vt, vn;

            int offset = 2, chars = 0;

            while (sscanf(line + offset, "%d/%d/%d%n", &v, &vt, &vn, &chars) == 3)
            {
                faces_vrt.push_back(v-1);
                faces_nrm.push_back(vn-1);
                faces_tex.push_back(vt-1);
                offset += chars;
            }

        }
    }

    fclose(file);

    // TGA loading uses a top-left origin; OBJ UVs use a bottom-left origin.
    if (normalmap.read_tga_file(normalmap_name)) normalmap.flip_vertically();
    else normalmap = {};

    if (texture.read_tga_file(texture_name)) texture.flip_vertically();
    else texture = {};
}


int Model::nverts() const
{
    return verts.size();
}

int Model::nfaces() const
{
    return static_cast<int>(faces_vrt.size() / 3);
}

Vertex Model::vert(int i) const
{
    return verts[i];
}

Vertex Model::vert(int iface, int nthvert) const
{
    return verts[faces_vrt[iface*3 + nthvert]];
}

vec3<double> Model::norm(int iface, int nthvert) const
{
    return norms[faces_nrm[iface*3 + nthvert]];
}

vec3<double> Model::norm_uv(vec2<double> uv) const
{
    if (!normalmap.width() || !normalmap.height()) return {0, 0, 1};
    int x = std::min(int(std::clamp(uv.x, 0.0, 1.0) * normalmap.width()), normalmap.width() - 1);
    int y = std::min(int(std::clamp(uv.y, 0.0, 1.0) * normalmap.height()), normalmap.height() - 1);
    TGAColor sample = normalmap.get(x, y);
    // RGB encodes object-space XYZ in [0,255]; TGAColor stores BGRA.
    return {sample[2] * (2.0 / 255.0) - 1.0,
            sample[1] * (2.0 / 255.0) - 1.0,
            sample[0] * (2.0 / 255.0) - 1.0};
}

TGAColor Model::tex(vec2<double> uv) const
{
    if (!texture.width() || !texture.height()) return {255, 255, 255, 255};
    int x = std::min(int(std::clamp(uv.x, 0.0, 1.0) * texture.width()), texture.width() - 1);
    int y = std::min(int(std::clamp(uv.y, 0.0, 1.0) * texture.height()), texture.height() - 1);
    return texture.get(x, y);
}

vec2<double> Model::uv(int iface, int nthvert) const
{
    return uvs[faces_tex[iface*3 + nthvert]];
}
