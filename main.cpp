#include <math.h>
#include "TgaImage.h"
#include "Geometry.h"
#include "Model.h"
#include "our_gl.h"

//////////////////
const int width = 1024;
const int height = 1024;
const TGAColor white(255,255,255,255);
Model *model;
Vec3f lightdir(0,0,1);
Vec3f camera(0,0, 10);

//////////////////
struct Shader : public IShader{
    virtual Vec3f vertex(int i , int j){
        Vec4f gl_vertex = embed<4>(model->Vert(model->Sampler(i, 0)[j]));
        uv[j] = model->Texcoord(model->Sampler(i, 1)[j]);
        normal[j] = model->Normal(i, j);
        gl_vertex = mvp * gl_vertex;
        return Vec3f(gl_vertex.x /gl_vertex.w, gl_vertex.y /gl_vertex.w, gl_vertex.z /gl_vertex.w);
    }
    virtual bool fragment(Vec3f barycentric){
        float x = normal[0][0] * barycentric[0] + normal[1][0] * barycentric[1] + normal[0][2] * barycentric[2];
        float y = normal[0][1] * barycentric[0] + normal[1][1] * barycentric[1] + normal[2][1] * barycentric[2];
        float z = normal[0][2] * barycentric[0] + normal[1][2] * barycentric[1] + normal[2][2] * barycentric[2];

        Vec3f n = Vec3f(x,y,z);
        n.normalize();
        float lambart = std::max(0.f, dot(n, lightdir));
        albedo = model->Diffuse(Sampler2D(uv, barycentric));
        albedo = TGAColor(albedo.r * lambart, albedo. g * lambart, albedo.b * lambart, albedo.a);
        return false;
    }
};
//////////////////
int main(int argc, const char * argv[]) {
    //新建zbuffer
    float *zbuffer = new float[width * height];
    for(int i = 0; i < width * height - 1; ++i){
        zbuffer[i] = -std::numeric_limits<int>::max();
    }
    //读取模型
    model = new Model("/Users/thirtytwo/Desktop/CppRasterizerRenderer/obj/african_head/african_head.obj");
    Vec3f rotation(0,0,0);
    //新建图片
    TGAImage image(width, height, TGAImage::RGB);
    //获取mvp矩阵
    get_model_matrix(rotation);
    get_view_matrix(camera);
    get_projection_matrix(45, 1, 0.1, 50);
    get_mvp_matrix();

    Shader shader;
    //获取模型顶点数
    for(int i = 0; i < model->AllFace(); ++i){
        Vec3f screen[3];
        for(int j = 0; j < 3; ++j){
            screen[j] = world2screen(shader.vertex(i, j), width, height);
        }
        Triangle(screen, shader, zbuffer, image);
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
