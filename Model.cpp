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
            for(int i = 0; i < 3; ++i) iss >>v.raw[i];
            verts_.push_back(v);
        }
        else if(!line.compare(0,2,"f ")){
            std::vector<int> f;
            int itrash, idx;
            iss >> trash;
            while(iss >> idx >> trash >> itrash >> trash >> itrash){
                //对于面存储的方式是 顶点坐标索引/纹理索引/法线索引
                //所以后两个itrash对应的就是这个顶点纹理坐标和法线坐标的索引
                idx--;
                f.push_back(idx);
            }
            faces_.push_back(f);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
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

Vec3f Model::Vert(int i) {
    return verts_[i];
}
