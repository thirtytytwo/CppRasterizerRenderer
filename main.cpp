#include "TgaImage.h"

int main(int argc, const char * argv[]) {
    
    const TGAColor white(255,255,255,255);
    TGAImage image(500,500,TGAImage::RGB);
    image.set(50, 65, white);
    
    image.write_tga_file("output.tga");
    return 0;
}
