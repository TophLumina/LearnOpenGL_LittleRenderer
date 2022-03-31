#include "Model.hpp"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

unsigned int TextureFromFile(const char *name, const std::string directory, bool needGammacorrection)
{
    std::string fileName = std::string(name);
    fileName = directory + '/' + fileName;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width;
    int height;
    int colorChannels;
    unsigned char *textureData = stbi_load(fileName.c_str(), &width, &height, &colorChannels, 0);

    if (textureData)
    {
        GLenum format;
        if (colorChannels == 1)
            format = GL_RED;
        else if (colorChannels == 3)
            format = GL_RGB;
        else if (colorChannels == 4)
            format = GL_RGBA;

        GLenum internalformat;
        if (colorChannels == 1)
            internalformat = GL_RED;
        else if (colorChannels == 3)
            internalformat = needGammacorrection ? GL_SRGB : GL_RGB;
        else if (colorChannels == 4)
            internalformat = needGammacorrection ? GL_SRGB_ALPHA : GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        stbi_image_free(textureData);
    }
    else
    {
        std::cout << "Texture Failed to Load at Path:" << fileName << std::endl;
        stbi_image_free(textureData);
    }

    return textureID;
}