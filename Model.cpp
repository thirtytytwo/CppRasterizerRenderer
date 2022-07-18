#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "Model.h"

Model::Model(const char *filename): verts_(), faces_(){
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if(in.fail()) return;
    std::string line;
    while(!in.eof()){
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if(!line.compare(0, 2, "v ")){
            iss>>trash;
            Vec3f v;
            for(int i = 0; i < 3; ++i) iss >>v[i];
            verts_.push_back(v);
        }
        else if(!line.compare(0, 3, "vt ")){
            iss >> trash >> trash;
            Vec3f v;
            for(int i = 0; i < 2; ++i) iss >>v[i];
            uv_.push_back(v);
        }
        else if(!line.compare(0,2,"f ")){
            std::vector<int> f;
            std::vector<int> coord;
            int itrash, idx, texIndex;
            iss >> trash;
            while(iss >> idx >> trash >> texIndex >> trash >> itrash){
                //对于面存储的方式是 顶点坐标索引/纹理索引/法线索引
                //所以后两个itrash对应的就是这个顶点纹理坐标和法线坐标的索引
                idx--;
                texIndex--;
                f.push_back(idx);
                coord.push_back(texIndex);
            }
            faces_.push_back(f);
            uv_Index.push_back(coord);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << "# uv#"<< uv_Index.size() << std::endl;
};

Model::~Model() {
}

int Model::AllVert() {
    return (int)verts_.size();
}

int Model::AllFace() {
    return (int)faces_.size();
}

std::vector<int> Model::Face(int idx) {
    return faces_[idx];
}

std::vector<int> Model::Sampler(int idx) {
    return uv_Index[idx];
}

Vec3f Model::Vert(int i) {
    return verts_[i];
}

Vec3f Model::Texcoord(int i){
    return uv_[i];
}
