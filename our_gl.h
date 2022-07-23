#include "TgaImage.h"
#include "Geometry.h"

extern Matrix m;
extern Matrix v;
extern Matrix p;
extern Matrix mvp;

void get_model_matrix(Vec3f rotation);
void get_view_matrix(int x, int y, int width, int height);
void get_projection_matrix(Vec3f camera);
void get_mvp_matrix();

struct IShader{
    virtual ~IShader();
    virtual Vec3f vertex(int i, int j) = 0;
    virtual bool fragment(Vec3f Barycentric) = 0;

    Vec3f NDC[3];
    Vec3f uv[3];
    Vec3f normal[3];
    TGAColor albedo;
};

Vec3f m2v(Matrix m);
mat<4, 1, float> v2m(Vec3f v);

Vec2f Sampler2D(Vec3f *uv, Vec3f bary_coord);
Vec3f world2screen(Vec3f v, const int width, const int height);
Vec3f Barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f p);
void Triangle(Vec3f *pts, IShader &shader, float *zbuffer, TGAImage &img);

void DrawLine(int x0, int y0, int x1, int y1, TGAImage &img, TGAColor color);
