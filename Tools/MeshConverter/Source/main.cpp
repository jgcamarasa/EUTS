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
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile("bird.dae",	aiProcess_ConvertToLeftHanded | 
															aiProcess_JoinIdenticalVertices | 
															aiProcess_Triangulate |
															aiProcess_GenNormals );

	FILE* outputFile;
	fopen_s(&outputFile, "../../../../Resources/Meshes/cube.mesh", "wb");
	
	fwrite(EUTS_ID, sizeof(char), sizeof(EUTS_ID), outputFile);
	fwrite(MESH_ID, sizeof(char), sizeof(MESH_ID), outputFile);
	printf("EUTS\n");
	printf("MESH\n");

	aiMesh *mesh = scene->mMeshes[0];

	// num vertex
	unsigned int numVerts = mesh->mNumVertices;
	fwrite(&numVerts, sizeof(unsigned int), 1, outputFile);
	printf("%u\n", numVerts);
	
	
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
	printf("%u\n", numIndices);
	for (unsigned int i = 0; i < numFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			fwrite(&(face.mIndices[j]), sizeof(unsigned int), 1, outputFile);
			printf("%u ", face.mIndices[j]);
		}
		printf("\n");
	}
	
	
	fclose(outputFile);

	return 0;
}
