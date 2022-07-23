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
        else if(!line.compare(0, 3, "vn ")){
            iss >> trash >> trash;
            Vec3f n;
            for(int i = 0; i < 3; ++i) iss >>n[i];
            norm_.push_back(n);
        }
        else if(!line.compare(0, 3, "vt ")){
            iss >> trash >> trash;
            Vec3f v;
            for(int i = 0; i < 2; ++i) iss >>v[i];
            uv_.push_back(v);
        }
        else if(!line.compare(0,2,"f ")){
            std::vector<Vec3i> f;
            Vec3i temp;//用temp中的xyz分量存储顶点，uv和法线
            int itrash;
            iss >> trash;
            while(iss >> temp[0] >> trash >> temp[1] >> trash >> temp[2]){
                //对于面存储的方式是 顶点坐标索引/纹理索引/法线索引
                //所以后两个itrash对应的就是这个顶点纹理坐标和法线坐标的索引
                for(int i = 0; i < 3; ++i) temp[i]--;
                f.push_back(temp);
            }
            faces_.push_back(f);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << "# uv#"<< uv_.size() << std::endl;
    load_texture(filename, "_diffuse.tga", _MainTex);
    load_texture(filename, "_nm_tangent.tga",      _NormalMap);
};

Model::~Model() {
}

void Model::load_texture(std::string filename, const char *suffix, TGAImage &img) {
    std::string texfile(filename);
    size_t dot = texfile.find_last_of(".");
    if (dot!=std::string::npos) {
        texfile = texfile.substr(0,dot) + std::string(suffix);
        std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        img.flip_vertically();
    }
}

int Model::AllFace() {
    return (int)faces_.size();
}

std::vector<int> Model::Sampler(int idx, int k) {
    std::vector<int> info;
    switch (k)
    {
    case 0:
        for(int i = 0; i < (int)faces_[idx].size(); ++i) info.push_back(faces_[idx][i][0]);
        return info;
        break;
    
    case 1:
        for(int i = 0; i < (int)faces_[idx].size(); ++i) info.push_back(faces_[idx][i][1]);
        return info;
        break;

    case 2:
        for(int i = 0; i < (int)faces_[idx].size(); ++i) info.push_back(faces_[idx][i][2]);
        return info;
        break;

    default:
        return info;
        break;
    }
}

Vec3f Model::Vert(int i) {
    return verts_[i];
}

Vec3f Model::Texcoord(int i){

    return uv_[i];
}

Vec3f Model::Normal(int i, int j){
    int idx = faces_[i][j][2];
    return norm_[idx];
}
Vec3f Model::Normal(Vec2f uvf) {
    Vec2i uv(uvf[0]*_NormalMap.get_width(), uvf[1]*_NormalMap.get_height());
    TGAColor c = _NormalMap.get(uv[0], uv[1]);
    Vec3f res;
    res[0] = (float)c.b/255.f*2.f - 1.f;
    res[1] = (float)c.g/255.f*2.f - 1.f;
    res[2] = (float)c.r/255.f*2.f - 1.f;
    return res;
}

TGAColor Model::Diffuse(Vec2f uv){
    return _MainTex.get(uv[0] * _MainTex.get_width(), uv[1] * _MainTex.get_height());
}
