#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "TgaImage.h"
#include "Geometry.h"

class Model{
private:
    std::vector<std::vector<Vec3i>> faces_;
    std::vector<Vec3f> verts_;
    std::vector<Vec3f> uv_;
    std::vector<Vec3f> norm_;

    TGAImage _MainTex;
    TGAImage _NormalMap;

    void load_texture(std::string filename, const char *suffix, TGAImage &img);

public:
    Model(const char *filename);
    ~Model();
    int AllFace();//所有面数
    Vec3f Vert(int index);//获取该索引列的顶点坐标
    Vec3f Texcoord(int index);//获取索引列的纹理坐标
    Vec3f Normal(int i, int j);
    TGAColor Diffuse(Vec2f uv);
    std::vector<int> Sampler(int index, int k);//获取该索引列对应的数据,k为数据类型
};

#endif //__MODEL_H__