#include "TgaImage.h"
#include "Geometry.h"

extern Matrix m;
extern Matrix v;
extern Matrix p;
extern Matrix mvp;

void get_model_matrix(Vec3f rotation);
void get_view_matrix(Vec3f view_translated);
void get_projection_matrix(float eye_fov, float aspect_ratio, float znear, float zfar);
void get_mvp_matrix();

struct IShader{
    virtual ~IShader();
    virtual Vec3f vertex(int i, int j) = 0;
    virtual bool fragment(Vec3f Barycentric) = 0;

    Vec3f uv[3];
    Vec3f lambart;
    TGAColor albedo;
};

Vec2f Sampler2D(Vec3f *uv, Vec3f bary_coord);
Vec3f world2screen(Vec3f v, const int width, const int height);
Vec3f Barycentric(Vec3f *pts, Vec3f p);
void Triangle(Vec3f *pts, IShader &shader, float *zbuffer, TGAImage &img);

void DrawLine(int x0, int y0, int x1, int y1, TGAImage &img, TGAColor color);
