#include "Model.h"

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include <iostream>
//using namespace std;

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


Model::Model(string const& path)
{
	loadModel(path);
}

void Model::loadModel(string const& path)
{
	Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
            path,
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_GenSmoothNormals |
            aiProcess_JoinIdenticalVertices);


    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        /*cout << "Mesh index: " << meshes.size() << endl;
        cout << "Vertices: " << mesh->mNumVertices << endl;
        cout << "Faces: " << mesh->mNumFaces << endl;*/

        Vertex vertex;

        vertex.Position.x = mesh->mVertices[i].x;
        vertex.Position.y = mesh->mVertices[i].y;
        vertex.Position.z = mesh->mVertices[i].z;

        vertex.Normal.x = mesh->mNormals[i].x;
        vertex.Normal.y = mesh->mNormals[i].y;
        vertex.Normal.z = mesh->mNormals[i].z;

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    return Mesh(vertices, indices);
}

void Model::Draw()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw();
}

void Model::Draw(GLuint shaderProgram, 
    float rotationBase, float rotationShoulder1, float rotationShoulder2)
{
    GLint modelLocation = glGetUniformLocation(shaderProgram, "model");

    
    glm::mat4 baseTransform = glm::mat4(1.0f);
    baseTransform = glm::rotate(
        baseTransform,
        glm::radians(rotationBase),
        glm::vec3(0.0f, 1.0f, 0.0f));

    
    
    glm::vec3 pivotShouder1(1.217f, 3.25f, -1.05f);
    glm::vec3 pivotShouder2(1.305f, 9.64f, 1.30f);
    glm::mat4 shoulder1Transform = glm::mat4(1.0f);
    glm::mat4 shoulder2Transform = glm::mat4(1.0f);

    shoulder1Transform = glm::translate(
        shoulder1Transform,
        pivotShouder1);

    shoulder1Transform = glm::rotate(
        shoulder1Transform,
        glm::radians(rotationShoulder1),
        glm::vec3(0.0f, 0.0f, 1.0f));

    shoulder1Transform = glm::translate(
        shoulder1Transform,
        -pivotShouder1);

    shoulder2Transform = glm::translate(
        shoulder2Transform,
        pivotShouder2);

    shoulder2Transform = glm::rotate(
        shoulder2Transform,
        glm::radians(rotationShoulder2),
        glm::vec3(0.0f, 0.0f, 1.0f));

    shoulder2Transform = glm::translate(
        shoulder2Transform,
        -pivotShouder2);

    glm::mat4 shoudler1World = baseTransform * shoulder1Transform;
    glm::mat4 shoudler2World = shoudler1World * shoulder2Transform;

    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        glm::mat4 model = glm::mat4(1.0f);

        switch (i)
        {
            case 1:
                model = baseTransform;
                break;

            case 2:
                model = baseTransform;
                break;

            case 3:
                model = baseTransform;
                break;

            case 4:
                model = shoudler1World;
                break;

            case 5:
                model = shoudler2World;
                break;

            default:
                break;
        }


        glUniformMatrix4fv(
            modelLocation,
            1,
            GL_FALSE,
            glm::value_ptr(model));

        meshes[i].Draw();
    }
}