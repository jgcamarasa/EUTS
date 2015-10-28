#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <stdio.h>

/* TODO 
- Commandline arguments

*/
// Format:
// 'E','U','T','S'
// 'M','E','S','H'
// numVerts		: unsigned int 
// for each vertex:
//	vertPos		: float[3]
//	vertUV		: float[2]
//	vertColor	: float[4]
// numIndices		: unsigned int
// indices		: unsigned int[numIndices] // faces * 3


char EUTS_ID[4] = { 'E', 'U', 'T', 'S' };
char MESH_ID[4] = { 'M', 'E', 'S', 'H' };

bool gWriteDebug = true;

void vector3D2Array(float* arr, aiVector3D vec)
{
	arr[0] = vec.x;
	arr[1] = vec.y;
	arr[2] = vec.z;
}

void color4D2Array(float* arr, aiColor4D col)
{
	arr[0] = col.r;
	arr[1] = col.g;
	arr[2] = col.b;
	arr[2] = col.a;
}

int main()
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile("cube.dae",	aiProcess_ConvertToLeftHanded | 
															aiProcess_JoinIdenticalVertices | 
															aiProcess_Triangulate |
															aiProcess_GenNormals );

	FILE* outputFile;
	fopen_s(&outputFile, "output.mesh", "wb");
	
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
		// vertex positions (x, y, z)
		float vertex[3];
		vector3D2Array(vertex, mesh->mVertices[i]);
		fwrite(vertex, sizeof(vertex), 1, outputFile);
		printf("%f %f %f\n", vertex[0], vertex[1], vertex[2]);

		// texture coordinates (u, v)
		float coord[3];
		vector3D2Array(coord, mesh->mTextureCoords[0][i]);
		fwrite(coord, sizeof(float) * 2, 1, outputFile);
		printf("%f %f\n", vertex[0], vertex[1]);

		// vertex color (r, g, b, a)
		float color[4];
		if (mesh->HasVertexColors(0))
		{
			color4D2Array(color, mesh->mColors[0][i]);
		}
		else
		{
			color[0] = 1.0f;
			color[1] = 1.0f;
			color[2] = 1.0f;
			color[3] = 1.0f;
		}

		fwrite(color, sizeof(color), 1, outputFile);
		printf("%f %f %f %f\n", color[0], color[1], color[2], color[3]);
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
