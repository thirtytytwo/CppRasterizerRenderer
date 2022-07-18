#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "Geometry.h"

class Model{
private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<int>> faces_;
    std::vector<std::vector<int>> uv_Index;
    std::vector<Vec3f> uv_;

public:
    Model(const char *filename);
    ~Model();
    int AllVert();//所有顶点数
    int AllFace();//所有面数
    Vec3f Vert(int index);//获取该索引列的顶点坐标
    Vec3f Texcoord(int index);//获取索引列的纹理坐标
    std::vector<int> Face(int index);//获取该索引列面对应顶点列的数据
    std::vector<int> Sampler(int index);//获取该索引列对应的纹理所在数据
};

#endif //__MODEL_H__