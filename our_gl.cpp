#include "our_gl.h"
#include <math.h>

const double PI = 3.1415926;

Matrix m;
Matrix v;
Matrix p;
Matrix mvp;

IShader::~IShader(){};

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
void get_view_matrix(Vec3f eye_pos){
    //还原摄像机平移
    Matrix view_M = Matrix::identity();
    view_M[0][0] = 1; view_M[0][1] = 0; view_M[0][2] = 0;    view_M[0][3] = -eye_pos[0];
    view_M[1][0] = 0; view_M[1][1] = 1; view_M[1][2] = 0;    view_M[1][3] = -eye_pos[1];
    view_M[2][0] = 0; view_M[2][1] = 0; view_M[2][2] = 1;    view_M[2][3] = -eye_pos[2];
    view_M[3][0] = 0; view_M[3][1] = 0; view_M[3][2] = 0;    view_M[3][3] = 1;
    v = view_M;
}
void get_projection_matrix(float eye_fov, float aspect_ratio, float znear, float zfar){
    float t = tan((eye_fov/360) * PI) * abs(znear);
    float r = t/aspect_ratio;

    Matrix m_p;
    m_p[0][0] = znear; m_p[0][1] = 0; m_p[0][2] = 0;    m_p[0][3] = 0;
    m_p[1][0] = 0; m_p[1][1] = znear; m_p[1][2] = 0;    m_p[1][3] = 0;
    m_p[2][0] = 0; m_p[2][1] = 0; m_p[2][2] = znear + zfar;    m_p[2][3] = -znear * zfar;
    m_p[3][0] = 0; m_p[3][1] = 0; m_p[3][2] = 0;    m_p[3][3] = 1;

    Matrix m_o_tran;
    m_o_tran[0][0] = 1; m_o_tran[0][1] = 0; m_o_tran[0][2] = 0;    m_o_tran[0][3] = 0;
    m_o_tran[1][0] = 0; m_o_tran[1][1] = 1; m_o_tran[1][2] = 0;    m_o_tran[1][3] = 0;
    m_o_tran[2][0] = 0; m_o_tran[2][1] = 0; m_o_tran[2][2] = 1;    m_o_tran[2][3] = -(znear + zfar)/2;
    m_o_tran[3][0] = 0; m_o_tran[3][1] = 0; m_o_tran[3][2] = 0;    m_o_tran[3][3] = 1;

    Matrix m_o_scale;
    m_o_scale[0][0] = 1/r; m_o_scale[0][1] = 0; m_o_scale[0][2] = 0;    m_o_scale[0][3] = 0;
    m_o_scale[1][0] = 0; m_o_scale[1][1] = 1/t; m_o_scale[1][2] = 0;    m_o_scale[1][3] = 0;
    m_o_scale[2][0] = 0; m_o_scale[2][1] = 0; m_o_scale[2][2] = 2/(znear - zfar);    m_o_scale[2][3] = 0;
    m_o_scale[3][0] = 0; m_o_scale[3][1] = 0; m_o_scale[3][2] = 0;    m_o_scale[3][3] = 1;

    Matrix projection = Matrix::identity();
    projection = (m_o_scale * m_o_tran) * m_p;
    p = projection;
}

void get_mvp_matrix(){
    mvp = p * v * m;
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
Vec3f Barycentric(Vec3f *pts, Vec3f p){
    Vec3f sx = Vec3f(pts[1][0] - pts[0][0], pts[2][0] - pts[0][0], pts[0][0] - p[0]);
    Vec3f sy = Vec3f(pts[1][1] - pts[0][1], pts[2][1] - pts[0][1], pts[0][1] - p[1]);

    Vec3f result = cross(sx, sy);
    if(std::abs(result.z) <= 0.) return Vec3f(-1,-1,-1);
    return Vec3f(1.f - (result.x + result.y) / result.z, result.y / result.z, result.x / result.z);
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
            Vec3f result = Barycentric(pts, p);
            shader.fragment(result);
            if(result.x < 0 || result.y < 0 || result.z < 0) continue;
            p.z = 0;
            for(int i = 0; i < 3; ++i) p.z += pts[i][2] * result[i];
            //写入深度缓存
            if(p.x + p.y * img.get_width() < 0) continue;
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
