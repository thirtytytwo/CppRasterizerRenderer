#include "TgaImage.h"
#include "Geometry.h"
#include "Model.h"

//////////////////
const int width = 800;
const int height = 800;
const TGAColor white(255,255,255,255);
Model *model;

///////////////////
Vec2f Sampler2D(Vec3f *uv, Vec3f bary_coord){
    float u = uv[0][0] * bary_coord[0] + uv[1][0] * bary_coord[1] + uv[2][0] * bary_coord[2];
    float v = uv[0][1] * bary_coord[0] + uv[1][1] * bary_coord[1] + uv[2][1] * bary_coord[2];
    return Vec2f(u,v);
}
Vec3f world2screen(Vec3f v) {
    return Vec3f(int((v.x+1.)*width/2.+.5), int((v.y+1.)*height/2.+.5), v.z);
}

//////////////////
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

Vec3f Barycentric(Vec3f *pts, Vec3f p){
    Vec3f sx = Vec3f(pts[1][0] - pts[0][0], pts[2][0] - pts[0][0], pts[0][0] - p[0]);
    Vec3f sy = Vec3f(pts[1][1] - pts[0][1], pts[2][1] - pts[0][1], pts[0][1] - p[1]);

    Vec3f result = cross(sx, sy);
    if(std::abs(result.z) < 1.) return Vec3f(-1,-1,-1);
    return Vec3f(1.f - (result.x + result.y) / result.z, result.y / result.z, result.x / result.z);
}
void Triangle(Vec3f *pts,Vec3f *uv, float *zbuffer,float lambart, TGAImage &img, TGAImage &albedo){
    //创建包围盒
    double min_x = std::min(pts[0][0], std::min(pts[1][0], pts[2][0]));
    double min_y = std::min(pts[0][1], std::min(pts[1][1], pts[2][1]));
    double max_x = std::max(pts[0][0], std::max(pts[1][0], pts[2][0]));
    double max_y = std::max(pts[0][1], std::max(pts[1][1], pts[2][1]));

    min_x = static_cast<int>(std::floor(min_x));
    min_y = static_cast<int>(std::floor(min_y));
    max_x = static_cast<int>(std::ceil(max_x));
    max_y = static_cast<int>(std::ceil(max_y));
    //检测包围盒内像素点是否在三角形内
    Vec3f p;
    for(p.x = min_x; p.x <= max_x; ++p.x){
        for(p.y = min_y; p.y <= max_y; ++p.y){
            Vec3f result = Barycentric(pts, p);
            if(result.x < 0 || result.y < 0 || result.z < 0) continue;
            p.z = 0;
            for(int i = 0; i < 3; ++i) p.z += pts[i][2] * result[i];
            //写入深度缓存
            if(zbuffer[int(p.x + p.y * width)] < p.z){
                zbuffer[int(p.x + p.y * width)] = p.z;
                Vec2f uv_result = Sampler2D(uv, result);
                TGAColor c = albedo.get(uv_result.x * albedo.get_width(), uv_result.y * albedo.get_height());
                img.set(p.x, p.y, TGAColor(c.r * lambart, c.g * lambart, c.b * lambart,c.a));
            }
        }
    }
}
//////////////////
int main(int argc, const char * argv[]) {
    //新建zbuffer
    float *zbuffer = new float[width * height];
    for(int i = 0; i < width * height - 1; ++i){
        zbuffer[i] = -std::numeric_limits<int>::max();
    }
    //读取模型
    model = new Model("/Users/thirtytwo/Desktop/CppRasterizerRenderer/obj/african_head/african_head.obj");
    //新建图片
    TGAImage image(width, height, TGAImage::RGB);
    //获取贴图
    TGAImage albedo(1024, 1024, TGAImage::RGB);
    albedo.read_tga_file("/Users/thirtytwo/Desktop/CppRasterizerRenderer/obj/african_head/african_head_diffuse.tga");
    albedo.flip_vertically();
    //新建光源
    Vec3f lightdir(0,0,-1);
    //获取模型顶点数
    for(int i = 0; i < model->AllFace(); ++i){
        std::vector<int> curFace = model->Face(i);
        std::vector<int> cur_uv = model->Sampler(i);
        Vec3f verts[3];
        Vec3f screen[3];
        Vec3f uv[3];
        for(int i = 0; i < 3; ++i){
            verts[i] = model->Vert(curFace[i]);
            uv[i] = model->Texcoord(cur_uv[i]);
            screen[i] = world2screen(verts[i]);
        }
        Vec3f n = cross(verts[2] - verts[0], verts[1] - verts[0]);
        n.normalize();
        float lambart = std::max(0.f, dot(lightdir, n));
        if(lambart > 0.)
            Triangle(screen, uv, zbuffer, lambart, image, albedo);
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
