#include "Entity.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Entity::Entity(){}




GLuint Entity::LoadTexture(const char* filePath){
    
    int w,h,comp;
    unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
    if(image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    GLuint newTexture;
    glGenTextures(1, &newTexture);
    glBindTexture(GL_TEXTURE_2D, newTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(image);
    return newTexture;
}

void Entity::resetPosition(){
    ModelMatrix.setPosition(x_init, y_init, 0.0);
    x = x_init;
    y = y_init;
}
