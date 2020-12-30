#ifndef H_MODEL
#define H_MODEL

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <assimp/scene.h>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoords;
};

enum TextureType
{
	DIFFUSE,
	SPECULAR
};

struct Texture
{
	unsigned int id;
	TextureType type;
	std::string path;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	unsigned int vao, vbo, ebo;
};

void setupMesh(Mesh &mesh);
void destroyMesh(Mesh &mesh);
void drawMesh(Mesh &mesh, unsigned int &shader);

struct Model
{
	std::vector<Mesh> meshes;
	std::string texturesDir;
	std::vector<Texture> sharedTextures;
};

void drawModel(Model &model, unsigned int &shader);
void destroyModel(Model &model);
std::vector<Texture> loadMaterialTextures(Model &model, aiMaterial *mat, aiTextureType assimpType, TextureType type);
Mesh processMesh(Model &model, aiMesh *mesh, const aiScene* scene);
void processNode(Model &model, aiNode *node, const aiScene *scene);
bool loadModel(Model &model, const char* path, const char* texturesDir);

#endif
