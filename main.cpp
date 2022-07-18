#include "TgaImage.h"
#include "Model.h"

void DrawLine(int x0, int y0, int x1, int y1, TGAImage &img, TGAColor color){
    bool isSwapXY = false;
    //统一让dx比dy大
    if(std::abs(x0 - x1) < std::abs(y0 - y1)){
        std::swap(x0, y0);
        std::swap(x1, y1);
        isSwapXY = true;
    }
    //为了避免斜率正负导致画线的区别，统一让x0x1和y0y1递增
    if(x0 > x1){
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    //斜率
    //float gradient = std::abs(dy/(float) dx);
    int gradient = std::abs(dy) * 2;
    int addon = 0;
    int y = y0;
    for(int x = x0; x<= x1; ++x){
        if(isSwapXY){
            img.set(y, x, color);
        }else img.set(x, y, color);
        addon += gradient;
        if(addon > dx){
            y += (y1 > y0 ? 1 : -1);
            addon -= dx * 2;
        }
    }
}

const int width = 800;
const int height = 800;
const TGAColor white(255,255,255,255);
Model *model;

int main(int argc, const char * argv[]) {
    //读取模型
    model = new Model("/Users/thirtytwo/Desktop/CppRasterizerRenderer/obj/african_head/african_head.obj");
    //新建图片
    TGAImage image(width, height, TGAImage::RGB);
    //获取模型顶点数
    for(int i = 0; i < model->AllFace(); ++i){
        std::vector<int> curFace = model->Face(i);
        std::vector<Vec3f> verts;
        for(int i = 0; i < 3; ++i) verts.push_back(model->Vert(curFace[i]));
        int x0 = (verts[0].x + 1.) * width / 2;
        int y0 = (verts[0].y + 1.) * height / 2;
        int x1 = (verts[1].x + 1.) * width / 2;
        int y1 = (verts[1].y + 1.) * height / 2;
        int x2 = (verts[2].x + 1.) * width / 2;
        int y2 = (verts[2].y + 1.) * height / 2;

        DrawLine(x0,y0,x1,y1,image,white);
        DrawLine(x1,y1,x2,y2,image,white);
        DrawLine(x2,y2,x0,y0,image,white);
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
