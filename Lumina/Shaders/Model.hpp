#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#ifdef _MODEL_DEBUG
#include <iostream>
#endif

#include "../Shader.hpp"
#include "Mesh.hpp"

unsigned int TextureFromFile(const char *path, const std::string directory, bool needGammacorrection);

class Model
{
public:
    Model(const char *path)
    {
        loadModel(path);
    }

    void Draw(Shader *shader)
    {
        for (Mesh amesh : meshes)
            amesh.Draw(shader);
    }

    void DrawbyInstance(Shader *shader, int num)
    {
        for (Mesh amesh : meshes)
            amesh.DrawbyInstance(shader, num);
    }

    // Used for Instance Rendering
    std::vector<Mesh> ServeMeshes()
    {
        return this->meshes;
    }

private:
    // Optimization
    std::vector<Texture> textures_loaded;

    // Meshs
    std::vector<Mesh> meshes;
    std::string directory;

    // funcs
    void loadModel(std::string path)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode *node, const aiScene *scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i)
            processNode(node->mChildren[i], scene);
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // Vertex
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex vertex;
            // get position, normal, texcoords and tangent
            glm::vec3 position;
            position.x = mesh->mVertices[i].x;
            position.y = mesh->mVertices[i].y;
            position.z = mesh->mVertices[i].z;
            vertex.Position = position;

            if (mesh->HasNormals())
            {
                glm::vec3 normal;
                normal.x = mesh->mNormals[i].x;
                normal.y = mesh->mNormals[i].y;
                normal.z = mesh->mNormals[i].z;
                vertex.Normal = normal;
            }

            if (mesh->mTextureCoords[0])
            {
                glm::vec2 texcoords;
                texcoords.x = mesh->mTextureCoords[0][i].x;
                texcoords.y = mesh->mTextureCoords[0][i].y;
                vertex.Texcoords = texcoords;

                // Tangent
                glm::vec3 tangent;
                tangent.x = mesh->mTangents[i].x;
                tangent.y = mesh->mTangents[i].y;
                tangent.z = mesh->mTangents[i].z;
                vertex.Tangent = tangent;

                //BiTangent
                glm::vec3 bitangent;
                bitangent.x = mesh->mBitangents[i].x;
                bitangent.y = mesh->mBitangents[i].y;
                bitangent.z = mesh->mBitangents[i].z;
                vertex.BiTangent = bitangent;
            }
            else
                vertex.Texcoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        // Indice
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace currentFace = mesh->mFaces[i];
            for (unsigned int j = 0; j < currentFace.mNumIndices; ++j)
                indices.push_back(currentFace.mIndices[j]);
        }

        // Material(Textures)
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

            std::vector<Texture> diffuseMaps = loadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse", true);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            std::vector<Texture> specularMaps = loadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular", false);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            std::vector<Texture> normalMaps = loadMaterialTexture(material, aiTextureType_NORMALS, "texture_normal", false);
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        }

#ifdef  _MODEL_DEBUG
        std::cout << std::endl;
        std::cout << "MANUAL_DEBUG::MESH_DATA" << std::endl;
        std::cout << "MANUAL_DEBUG::MESH_DATA::VERTEX" << std::endl;
        for (Vertex avertex : vertices)
            std::cout << "\tPosition::" << '(' << avertex.Position.x << ', ' << avertex.Position.y << ', ' << avertex.Position.z << ')' << std::endl;
        std::cout << vertices.size() << " Verteices Loaded." << std::endl;
        std::cout << indices.size() << " Indices Loaded." << std::endl;
        std::cout << textures.size() << " Textures Loaded." << std::endl;
        std::cout << std::endl;
#endif

        return Mesh(vertices, indices, textures);
    }

    std::vector<Texture> loadMaterialTexture(aiMaterial *material, aiTextureType type, std::string typeName, bool needGammacorrection)
    {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < material->GetTextureCount(type); ++i)
        {
            aiString texturePath;
            material->GetTexture(type, i, &texturePath);

            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); ++j)
            {
                if (std::strcmp(textures_loaded[j].path.data, texturePath.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }

            if (!skip)
            {
                Texture texture;
                texture.id = TextureFromFile(texturePath.C_Str(), directory, needGammacorrection);
                texture.type = typeName;
                texture.path = texturePath;
                textures.push_back(texture);
                textures_loaded.push_back(texture); // add the texture in the textures_loaded vector
            }
        }

        return textures;
    }
};