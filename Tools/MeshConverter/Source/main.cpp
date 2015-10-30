#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "../../../Source/mesh.h"
#include <stdio.h>

/* TODO 
- Commandline arguments

*/
// Format:
// 'E','U','T','S'
// 'M','E','S','H'
// numVerts		: unsigned int 
// vertices		: EUTS_Vertex[numVerts]
// numIndices	: unsigned int
// indices		: unsigned int[numIndices] // faces * 3


char EUTS_ID[4] = { 'E', 'U', 'T', 'S' };
char MESH_ID[4] = { 'M', 'E', 'S', 'H' };

bool gWriteDebug = true;


int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("MeshConvertex.exe inputFile outputFile\n");
		return -1;
	}

	const char *input = argv[1];
	const char *output = argv[2];

	printf("Processing %s\n", input);

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(input, aiProcess_ConvertToLeftHanded |
															aiProcess_JoinIdenticalVertices | 
															aiProcess_Triangulate |
															aiProcess_GenNormals );
	if (!scene)
	{
		printf("Couldn't open input file: %s\n", input);
		return -1;
	}

	aiMesh *mesh = scene->mMeshes[0];

	if (!mesh)
	{
		printf("This file doesn't have a mesh\n");
		return -1;
	}

	if (!mesh->HasTextureCoords(0))
	{
		printf("Mesh %s doesn't have texture coords\n", mesh->mName.C_Str());
	}
	FILE* outputFile;
	errno_t err = fopen_s(&outputFile, output, "wb");
	if (err == EINVAL)
	{
		printf("Couldn't open output file: %s\n", output);
		return -1;
	}
	
	fwrite(EUTS_ID, sizeof(char), sizeof(EUTS_ID), outputFile);
	fwrite(MESH_ID, sizeof(char), sizeof(MESH_ID), outputFile);

	// num vertex
	unsigned int numVerts = mesh->mNumVertices;
	fwrite(&numVerts, sizeof(unsigned int), 1, outputFile);
	
	for (unsigned int i = 0; i < numVerts; ++i)
	{
		EUTS_Vertex vertex;
		
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;
		
		vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
		vertex.texCoord.y = mesh->mTextureCoords[0][i].y;

		fwrite(&vertex, 1, sizeof(vertex), outputFile);
	}

	

	// Num faces
	unsigned int numFaces = mesh->mNumFaces;
	unsigned int numIndices = numFaces * 3;
	fwrite(&numIndices, sizeof(unsigned int), 1, outputFile);
	for (unsigned int i = 0; i < numFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			fwrite(&(face.mIndices[j]), sizeof(unsigned int), 1, outputFile);
		}
	}
	
	
	fclose(outputFile);

	return 0;
}
