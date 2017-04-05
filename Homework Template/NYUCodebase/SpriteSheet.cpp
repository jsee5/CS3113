#include "SpriteSheet.h"


SpriteSheet::SpriteSheet(GLuint textureID, int index, int sprite_count_x, int sprite_count_y, float size): textureID(textureID), index(index), size(size){
    u = (float)(((int)index) % sprite_count_x) / (float) sprite_count_x;
    v = (float)(((int)index) / sprite_count_x) / (float) sprite_count_y;
    width = 1.0/(float)sprite_count_x;
    height = 1.0/(float)sprite_count_y;
    //    aspect = width / height;
}

void SpriteSheet::draw(ShaderProgram*& program) {
    
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    GLfloat texCoords[] = {
        u,          v + height,
        u + width,  v,
        u,          v,
        u + width,  v,
        u,          v + height,
        u + width,  v + height
    };
    
    float vertices[] = {
        -0.5f * size,  -0.5f * size,
        0.5f * size ,   0.5f * size,
        -0.5f * size,  0.5f * size,
        0.5f * size,   0.5f * size,
        -0.5f * size,  -0.5f * size ,
        0.5f * size,   -0.5f * size
    };
    
    
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    
    
    
}
