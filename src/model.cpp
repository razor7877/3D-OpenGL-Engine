#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "model.hpp"

Model::Model()
{

}

Model::Model(std::string path, GLuint shaderProgramID)
{
	this->shaderProgramID = shaderProgramID;
	loadModel(path);
}

void Model::drawObject()
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].drawObject();
	}
}

void Model::setupObject()
{
	// Do NOT iterate over the meshes using a foreach loop such as:
	// for (Mesh mesh : meshes)
	// Doing so causes the generated data (VAOs, VBOs etc.) to be lost when going out of scope
	// Which results in memory access violations
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].setupObject();
	}
}

void Model::loadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// Process all the node's meshes (if any)
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
	std::vector<float> vertices;
	std::vector<float> texCoords;
	std::vector<float> normals;
	std::vector<unsigned int> indices;

	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.push_back(mesh->mVertices[i].x);
		vertices.push_back(mesh->mVertices[i].y);
		vertices.push_back(mesh->mVertices[i].z);

		if (mesh->HasNormals())
		{
			normals.push_back(mesh->mNormals[i].x);
			normals.push_back(mesh->mNormals[i].y);
			normals.push_back(mesh->mNormals[i].z);
		}

		if (mesh->mTextureCoords[0])
		{
			texCoords.push_back(mesh->mTextureCoords[0][i].x);
			texCoords.push_back(mesh->mTextureCoords[0][i].y);
		}
		else
		{
			texCoords.push_back(0.0f);
			texCoords.push_back(0.0f);
		}
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	return Mesh(vertices, texCoords, normals, indices, textures, shaderProgramID);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		std::string path = directory + '/' + std::string(str.C_Str());

		Texture texture = Texture(path, typeName, false);
		texture.path = str.C_Str();
		textures.push_back(texture);
	}

	return textures;
}

Model& Model::rotateModel(float degrees, glm::vec3 rotationPoint)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].rotateMesh(degrees, rotationPoint);
	}
	return *this;
}

Model& Model::rotateModel(float degrees, float x, float y, float z)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].rotateMesh(degrees, x, y, z);
	}
	return *this;
}

Model& Model::translateModel(glm::vec3 translation)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].translateMesh(translation);
	}
	return *this;
}

Model& Model::translateModel(float x, float y, float z)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].translateMesh(x, y, z);
	}
	return *this;
}

Model& Model::scaleModel(glm::vec3 scaleVec)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].scaleMesh(scaleVec);
	}
	return *this;
}

Model& Model::scaleModel(float scaleX, float scaleY, float scaleZ)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].scaleMesh(scaleX, scaleY, scaleZ);
	}
	return *this;
}