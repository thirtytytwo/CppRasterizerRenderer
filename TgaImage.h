//
//  TgaImage.hpp
//  CppRasterizerRenderer
//
//  Created by thirtytwo on 2022/7/17.
//

#ifndef TgaImage_hpp
#define TgaImage_hpp

#include <fstream>

#pragma pack(push,1)
struct TGA_Header{
    char idlength;
    char colormaptype;
    char datatypecode;
    short colormaporigin;
    short colormaplength;
    char colormapdepth;
    short x_origin;
    short y_origin;
    short width;
    short height;
    char bitsperpixel;
    char imagedescriptor;
};
#pragma pack(pop)


struct TGAColor{
    union{
        struct{
            unsigned char r, g, b, a;
        };
        unsigned char raw[4];
        unsigned int val;
    };
    int bytespp;
    
    TGAColor() : val(0), bytespp(1){}
    TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A): r(R), g(G), b(B), a(A), bytespp(4){}
    TGAColor(const TGAColor& color): val(color.val), bytespp(color.bytespp){}
    TGAColor(unsigned char *p, int bpp): val(0), bytespp(bpp){
        for(int i = 0; i < bpp; ++i){
            raw[i] = p[i];
        }
    }
    
    TGAColor & operator =(const TGAColor &c){
        if(this != &c){
            bytespp = c.bytespp;
            val = c.val;
        }
        return *this;
    }
};

class TGAImage{
protected:
    unsigned char *data;
    int width;
    int height;
    int bytespp;
    
    bool load_rle_data(std::ifstream &in);
    bool unload_rle_data(std::ofstream &out);
    
public:
    enum Format{
        GRAYSCALE = 1,
        RGB = 3,
        RGBA = 4
    };
    
    TGAImage();
    TGAImage(int w, int h ,int bpp);
    TGAImage(const TGAImage &image);
    
    bool read_tga_file(const char *filename);
    bool write_tga_file(const char *filename, bool rle = true);
    bool flip_horizontally();
    bool flip_vertically();
    bool scale(int w, int h);
    TGAColor get(int x, int y);
    bool set(int x, int y, TGAColor c);
    ~TGAImage();
    TGAImage & operator =(const TGAImage &image);
    int get_width();
    int get_height();
    int get_bytespp();
    unsigned char *buffer();
    void clear();
};

#endif /* TgaImage_hpp */
