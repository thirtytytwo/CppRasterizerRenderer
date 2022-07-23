#include "our_gl.h"
#include <math.h>

const double PI = 3.1415926;

Matrix m;
Matrix v;
Matrix p;
Matrix mvp;

IShader::~IShader(){};

Vec3f m2v(Matrix m){
    return Vec3f(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
}

mat<4, 1, float> v2m(Vec3f v){
    mat<4, 1, float> m;
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
}

void get_model_matrix(Vec3f rotation){
    float angleZ = (rotation.z / 180.f) *PI;
    float angleY = (rotation.y / 180.f) *PI;
    float angleX = (rotation.x / 180.f) *PI;
    float cosx = cos(angleX);
    float cosy = cos(angleY);
    float cosz = cos(angleZ);
    float sinx = sin(angleX);
    float siny = sin(angleY);
    float sinz = sin(angleZ);

    //绕z轴旋转
    Matrix model_M_z = Matrix::identity();
    model_M_z[0][0] = cosz; model_M_z[0][1] = -sinz; model_M_z[0][2] = 0;    model_M_z[0][3] = 0;
    model_M_z[1][0] = sinz; model_M_z[1][1] = cosz;  model_M_z[1][2] = 0;    model_M_z[1][3] = 0;
    model_M_z[2][0] = 0;    model_M_z[2][1] = 0;     model_M_z[2][2] = 1;    model_M_z[2][3] = 0;
    model_M_z[3][0] = 0;    model_M_z[3][1] = 0;     model_M_z[3][2] = 0;    model_M_z[3][3] = 1;

    //绕y轴旋转
    Matrix model_M_y = Matrix::identity();
    model_M_y[0][0] = cosy; model_M_y[0][1] = 0; model_M_y[0][2] = -siny;   model_M_y[0][3] = 0;
    model_M_y[1][0] = 0;    model_M_y[1][1] = 1; model_M_y[1][2] = 0;       model_M_y[1][3] = 0;
    model_M_y[2][0] = siny; model_M_y[2][1] = 0; model_M_y[2][2] = cosy;    model_M_y[2][3] = 0;
    model_M_y[3][0] = 0;    model_M_y[3][1] = 0; model_M_y[3][2] = 0;       model_M_y[3][3] = 1;
    //绕x轴旋转
    Matrix model_M_x = Matrix::identity();
    model_M_x[0][0] = 1; model_M_x[0][1] = 0;    model_M_x[0][2] = 0;       model_M_x[0][3] = 0;
    model_M_x[1][0] = 0; model_M_x[1][1] = cosx; model_M_x[1][2] = -sinx;   model_M_x[1][3] = 0;
    model_M_x[2][0] = 0; model_M_x[2][1] = sinx; model_M_x[2][2] = cosx;    model_M_x[2][3] = 0;
    model_M_x[3][0] = 0; model_M_x[3][1] = 0;    model_M_x[3][2] = 0;       model_M_x[3][3] = 1;
    m = model_M_z * model_M_y * model_M_x;
}
void get_view_matrix(int x, int y, int width, int height){
    Matrix view_M = Matrix::identity();
    view_M[0][3] = x + width * .5f;
    view_M[1][3] = y + height * .5f;
    view_M[2][3] = 255.f * .5;

    view_M[0][0] = width * .5f;
    view_M[1][1] = height * .5f;
    view_M[2][2] = 255 * .5f;
    v = view_M;
}
void get_projection_matrix(Vec3f camera){
    Matrix projection = Matrix::identity();
    projection[3][2] = -1.f/camera.z;
    p = projection;
}

void get_mvp_matrix(){
    mvp = v * p;
}

//////////////////triangle//////////////////
Vec2f Sampler2D(Vec3f *uv, Vec3f bary_coord){
    float u = uv[0][0] * bary_coord[0] + uv[1][0] * bary_coord[1] + uv[2][0] * bary_coord[2];
    float v = uv[0][1] * bary_coord[0] + uv[1][1] * bary_coord[1] + uv[2][1] * bary_coord[2];
    return Vec2f(u,v);
}
Vec3f world2screen(Vec3f v, int width, int height) {
    return Vec3f(int((v.x+1.)*width * 0.5), int((v.y+1.)*height * 0.5), v.z * (100 - 0.1) / 2.0 + (100 + 0.1) / 2.0);
}
Vec3f Barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
    Vec3f s[2];
    for (int i=2; i--; ) {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return Vec3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void Triangle(Vec3f *pts, IShader &shader, float *zbuffer, TGAImage &img){
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
            Vec3f result = Barycentric(pts[0],pts[1], pts[2], p);
            if(result.x < 0 || result.y < 0 || result.z < 0) continue;
            shader.fragment(result);
            p.z = 0;
            for(int i = 0; i < 3; ++i) p.z += pts[i][2] * result[i];
            //写入深度缓存
            if(zbuffer[int(p.x + p.y * img.get_width())] < p.z){
                zbuffer[int(p.x + p.y * img.get_width())] = p.z;
                img.set(p.x, p.y, shader.albedo);
            }
        }
    }
}

///////////////////Line///////////////
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
