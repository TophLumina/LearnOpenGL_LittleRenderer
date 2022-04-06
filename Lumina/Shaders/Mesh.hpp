#pragma once
#include <glad/glad.h>
#include <assimp/Importer.hpp>

#include <vector>

#include "glm/glm.hpp"
#include "../Shader.hpp"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 Texcoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    aiString path;
};

class Mesh {
public:
    // Mesh
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    // Function
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        setpuMesh();
    }

    void Draw(Shader *shader) {
        loadTextures(shader);

        // draw Mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, 0);

        // set otherthings back to defaults
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }

    void DrawbyInstance(Shader *shader, unsigned int num) {
        loadTextures(shader);

        glBindVertexArray(VAO);
        // glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, 0);
        // Using Func::glDrawElementsInstanced() for Instance Rendering
        glDrawElementsInstanced(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, 0, num);

        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }

    // Used for Instance Rendering
    unsigned int ServeVAO() {
        return this->VAO;
    }

private:
    // Render Data
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    void setpuMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Vertex Pos
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);    // easy to find that the stride is a Vertex

        // Vertex Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex,Normal));   // the func offsetof(structure,member) can get the offset of a member in a <struct>. and this is because the memery layout of <struct> is sequential for all its items
        
        // Vertex Texcoord
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Texcoords));

        glBindVertexArray(0);
    }

    void loadTextures(Shader *shader) {
        // the naming rule should fellows texture_ + diffuse/specular/*** + 1/2/3   <the numbers starts from 1>
        // Sample: texture_diffuse1; texture_specular2;
        unsigned int diffuseIndex = 1;
        unsigned int specularIndex = 1;
        for (unsigned int i = 0; i < textures.size(); ++i) {
            // before binding the texture we need to make it active first

            // The order of the Sampler index has changed for Environment Mapping.
            // GL_TEXTURE0 ~ 4 is reserved for extera textures.
            glActiveTexture(GL_TEXTURE5 + i);
            // then get its Index number
            std::string number;
            std::string name = textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseIndex++);
            else if(name == "texture_specular")
                number = std::to_string(specularIndex++);

            shader->setInt(("material." + name + number), i + 5);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
    }
};