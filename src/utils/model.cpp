#include <assimp/cimport.h>        // Plain-C interface
#include <assimp/postprocess.h>
#include <glad/glad.h>

#include "./model.h"
#include "./texture.h"

void destroyMesh(Mesh &mesh)
{
	glDeleteVertexArrays(1, &(mesh.vao));
	glDeleteBuffers(1, &(mesh.vbo));
	glDeleteBuffers(1, &(mesh.ebo));
	for (unsigned int i = 0; i < mesh.textures.size(); ++i)
	{
		glDeleteTextures(1, &(mesh.textures[i].id));
	}
}

void setupMesh(Mesh &mesh)
{
	glGenVertexArrays(1, &(mesh.vao));
	glGenBuffers(1, &(mesh.vbo));
	glGenBuffers(1, &(mesh.ebo));

	glBindVertexArray(mesh.vao);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &(mesh.vertices[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &(mesh.indices[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));
}

void drawMesh(Mesh &mesh, unsigned int &shader)
{
	glUseProgram(shader);
	unsigned int diffuseN = 0, specularN = 0;
	for (unsigned int i = 0; i < mesh.textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		bool isDiffuse = mesh.textures[i].type == DIFFUSE;
		std::string name(isDiffuse ? "diffuse_" : "specular_");
		name += std::to_string(isDiffuse ? diffuseN : specularN);
		glUniform1i(glGetUniformLocation(shader, ("material." + name).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);
		diffuseN += isDiffuse ? 1 : 0;
		specularN += isDiffuse ? 0 : 1;
	}
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(mesh.vao);
	glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void drawModel(Model &model, unsigned int &shader)
{
	for (unsigned int i = 0; i < model.meshes.size(); ++i)
	{
		drawMesh(model.meshes[i], shader);
	}
}

void destroyModel(Model &model)
{
	for (unsigned int i = 0; i < model.meshes.size(); ++i)
	{
		destroyMesh(model.meshes[i]);
	}
}

std::vector<Texture> loadMaterialTextures(Model &model, aiMaterial *mat, aiTextureType assimpType, TextureType type)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(assimpType); ++i)
	{
		aiString str;
		mat->GetTexture(assimpType, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < model.sharedTextures.size(); ++j)
		{
			if (std::strcmp(model.sharedTextures[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(model.sharedTextures[j]);
				skip = true;
				break;
			}
		}
		
		if (!skip)
		{
			Texture texture;
			std::string fullPath = model.texturesDir + str.C_Str();
			printf("load texture: %s\n", fullPath.c_str());
			texture.id = texture::loadTexture(fullPath.c_str(), true);
			texture.type = type;
			texture.path = str.C_Str();
			textures.push_back(texture);
			model.sharedTextures.push_back(texture);
		}
	}

	return textures;
}

Mesh processMesh(Model &model, aiMesh *mesh, const aiScene* scene)
{
	Mesh m;
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		glm::vec3 position;
		position.x = mesh->mVertices[i].x;
		position.y = mesh->mVertices[i].y;
		position.z = mesh->mVertices[i].z;
		vertex.position = position;

		glm::vec3 normal;
		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;
		vertex.normal = normal;

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.textureCoords = vec;
		}
		else
		{
			vertex.textureCoords = glm::vec2(0.0f, 0.0f);
		}

		m.vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			m.indices.push_back(face.mIndices[j]);
		}
	}

	if(mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = loadMaterialTextures(model, material, aiTextureType_DIFFUSE, DIFFUSE);
		m.textures.insert(m.textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = loadMaterialTextures(model, material, aiTextureType_SPECULAR, SPECULAR);
		m.textures.insert(m.textures.end(), specularMaps.begin(), specularMaps.end());
	}

	setupMesh(m);

	return m;
}

void processNode(Model &model, aiNode *node, const aiScene *scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		model.meshes.push_back(processMesh(model, mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		processNode(model, node->mChildren[i], scene);
	}
}

bool loadModel(Model &model, const char* path, const char* texturesDir)
{
	const aiScene* scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (scene == NULL)
	{
		printf("ERR::ASSIMP: %s\n", aiGetErrorString());
		return false;
	}

	model.texturesDir = texturesDir;
	processNode(model, scene->mRootNode, scene);

	aiReleaseImport(scene);

	return true;
}
